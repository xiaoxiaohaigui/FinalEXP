/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    dht11.c
 * @brief   DHT11 driver implementation.
 ******************************************************************************
 */
/* USER CODE END Header */

#include "dht11.h"
#include "DelayUs.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

static int
DHT11_WaitForState(GPIO_PinState state, uint16_t timeout_us)
{
    uint16_t elapsed = 0;

    while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) != state)
    {
        if(elapsed++ >= timeout_us)
        {
            return 0;
        }
        Delay_us(1);
    }
    return 1;
}

/* 等待指定电平结束（带超时，单位 us） */
static int
DHT11_WaitLevelEnd(GPIO_PinState state, uint16_t timeout_us)
{
    uint16_t elapsed = 0;

    while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == state)
    {
        if(elapsed++ >= timeout_us)
        {
            return 0;
        }
        Delay_us(1);
    }
    return 1;
}

/* 发送起始信号 */
static void
DHT11_Start_Signal(void)
{
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
    Delay_us(18000);
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    Delay_us(30);
}

/* 检查从机响应 */
static int
DHT11_Check_Response(void)
{
    if(!DHT11_WaitForState(GPIO_PIN_RESET, 100))
    {
        return 0;
    }
    if(!DHT11_WaitLevelEnd(GPIO_PIN_RESET, 100))
    {
        return 0;
    }
    if(!DHT11_WaitForState(GPIO_PIN_SET, 100))
    {
        return 0;
    }
    if(!DHT11_WaitLevelEnd(GPIO_PIN_SET, 100))
    {
        return 0;
    }

    return 1;
}

/* 读取 1 字节数据 */
static int
DHT11_Read_Byte(uint8_t *out)
{
    uint8_t byte = 0;

    if(out == NULL)
    {
        return 0;
    }

    for(int i = 0; i < 8; i++)
    {
        if(!DHT11_WaitForState(GPIO_PIN_RESET, 70))
        {
            return 0;
        }
        if(!DHT11_WaitLevelEnd(GPIO_PIN_RESET, 70))
        {
            return 0;
        }
        if(!DHT11_WaitForState(GPIO_PIN_SET, 70))
        {
            return 0;
        }

        Delay_us(35);
        byte <<= 1;
        if(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
        {
            byte |= 0x01;
        }
        if(!DHT11_WaitLevelEnd(GPIO_PIN_SET, 100))
        {
            return 0;
        }
    }

    *out = byte;
    return 1;
}

/* 初始化驱动（依赖外部已配置 GPIO） */
void
DHT11_Init(void)
{
    Delay_Init();
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    Delay_us(1000);
}

/* 读取温湿度数据，成功返回 0 */
int
DHT11_Read(DHT11_Data *out)
{
    uint8_t buf[5] = { 0 };

    if(out == NULL)
    {
        return -3;
    }

    /* 起始信号: 18ms 低电平 (仅需满足最小时长，允许中断) */
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
    Delay_us(18000);

    /* 关闭全局中断，保护后续时序关键的应答与数据读取 */
    __disable_irq();

    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    Delay_us(30);

    if(!DHT11_Check_Response())
    {
        __enable_irq();
        return -1;
    }

    for(int i = 0; i < 5; i++)
    {
        if(!DHT11_Read_Byte(&buf[i]))
        {
            __enable_irq();
            return -1;
        }
    }

    __enable_irq();

    if((uint8_t)(buf[0] + buf[1] + buf[2] + buf[3]) != buf[4])
    {
        return -2;
    }

    out->humidity_int = buf[0];
    out->humidity_dec = buf[1];
    out->temperature_int = buf[2];
    out->temperature_dec = buf[3];

    return 0;
}

/* 使用 UART1 DMA 发送测量值 */
HAL_StatusTypeDef
DHT11_SendDma(const DHT11_Data *data)
{
    static char tx_buf[64];
    int len = 0;

    if(data == NULL)
    {
        return HAL_ERROR;
    }

    len = snprintf(tx_buf,
                   sizeof(tx_buf),
                   "DHT11 H:%u.%u T:%u.%u\r\n",
                   (unsigned int)data->humidity_int,
                   (unsigned int)data->humidity_dec,
                   (unsigned int)data->temperature_int,
                   (unsigned int)data->temperature_dec);

    if(len <= 0)
    {
        return HAL_ERROR;
    }
    if((size_t)len > sizeof(tx_buf))
    {
        len = sizeof(tx_buf);
    }

    return HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_buf, (uint16_t)len);
}

/* 读取并通过 DMA 发送，成功返回 0 */
int
DHT11_ReadAndSendDma(void)
{
    DHT11_Data data;
    int rc = DHT11_Read(&data);

    if(rc != 0)
    {
        return rc;
    }

    if(DHT11_SendDma(&data) != HAL_OK)
    {
        return -4;
    }

    return 0;
}
