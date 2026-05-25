/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    ds18b20.c
 * @brief   DS18B20 driver implementation.
 ******************************************************************************
 */
/* USER CODE END Header */

#include "ds18b20.h"
#include "DelayUs.h"
#include "cmsis_os.h"
#include "usart.h"
#include <stdio.h>

#define DS18B20_CMD_SKIP_ROM 0xCC
#define DS18B20_CMD_WRITE_SCRATCHPAD 0x4E
#define DS18B20_CMD_CONVERT 0x44
#define DS18B20_CMD_READ_SCRATCHPAD 0xBE
#define DS18B20_CONV_TIME_MS 94
#define DS18B20_CFG_9BIT 0x1F

static void
DS18B20_DriveLow(uint16_t us)
{
    HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
    if(us > 0)
    {
        Delay_us(us);
    }
}

static void
DS18B20_Release(uint16_t us)
{
    HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_SET);
    if(us > 0)
    {
        Delay_us(us);
    }
}

/* 复位并检测存在脉冲，成功返回 1 */
static int
DS18B20_Reset(void)
{
    DS18B20_DriveLow(500);
    DS18B20_Release(70);

    if(HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin) != GPIO_PIN_RESET)
    {
        DS18B20_Release(410);
        return 0;
    }

    DS18B20_Release(410);
    return 1;
}

/* 写 1 位数据 */
static void
DS18B20_WriteBit(uint8_t bit)
{
    if(bit)
    {
        DS18B20_DriveLow(6);
        DS18B20_Release(64);
    }
    else
    {
        DS18B20_DriveLow(60);
        DS18B20_Release(10);
    }
}

/* 读 1 位数据 */
static uint8_t
DS18B20_ReadBit(void)
{
    uint8_t bit = 0;

    DS18B20_DriveLow(6);
    DS18B20_Release(9);
    bit = (HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin) == GPIO_PIN_SET) ? 1U : 0U;
    Delay_us(55);

    return bit;
}

/* 写 1 字节（LSB first） */
static void
DS18B20_WriteByte(uint8_t data)
{
    for(int i = 0; i < 8; i++)
    {
        DS18B20_WriteBit(data & 0x01U);
        data >>= 1U;
    }
}

/* 读 1 字节（LSB first） */
static uint8_t
DS18B20_ReadByte(void)
{
    uint8_t data = 0;

    for(int i = 0; i < 8; i++)
    {
        uint8_t bit = DS18B20_ReadBit();
        data |= (uint8_t)(bit << i);
    }

    return data;
}

void
DS18B20_Init(void)
{
    Delay_Init();
    DS18B20_Release(1000);
}

int
DS18B20_Read(DS18B20_Data *out)
{
    uint8_t lsb = 0;
    uint8_t msb = 0;
    int16_t raw = 0;
    int32_t temp_x10 = 0;

    if(out == NULL)
    {
        return -3;
    }

    if(!DS18B20_Reset())
    {
        return -1;
    }

    DS18B20_WriteByte(DS18B20_CMD_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_CMD_WRITE_SCRATCHPAD);
    DS18B20_WriteByte(0x4B);
    DS18B20_WriteByte(0x46);
    DS18B20_WriteByte(DS18B20_CFG_9BIT);

    if(!DS18B20_Reset())
    {
        return -1;
    }

    DS18B20_WriteByte(DS18B20_CMD_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_CMD_CONVERT);

    osDelay(DS18B20_CONV_TIME_MS);

    if(!DS18B20_Reset())
    {
        return -1;
    }

    DS18B20_WriteByte(DS18B20_CMD_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_CMD_READ_SCRATCHPAD);

    lsb = DS18B20_ReadByte();
    msb = DS18B20_ReadByte();

    raw = (int16_t)((msb << 8) | lsb);
    temp_x10 = (int32_t)raw * 10 / 16;

    out->temperature_x10 = (int16_t)temp_x10;

    return 0;
}

HAL_StatusTypeDef
DS18B20_SendDma(const DS18B20_Data *data)
{
    static char tx_buf[48];
    int len = 0;
    int16_t temp_x10 = 0;
    int16_t abs_x10 = 0;
    int16_t int_part = 0;
    int16_t dec_part = 0;
    char sign = '+';

    if(data == NULL)
    {
        return HAL_ERROR;
    }

    temp_x10 = data->temperature_x10;
    if(temp_x10 < 0)
    {
        sign = '-';
        abs_x10 = (int16_t)(-temp_x10);
    }
    else
    {
        abs_x10 = temp_x10;
    }

    int_part = (int16_t)(abs_x10 / 10);
    dec_part = (int16_t)(abs_x10 % 10);

    len =
      snprintf(tx_buf, sizeof(tx_buf), "DS18B20 T:%c%d.%d\r\n", sign, (int)int_part, (int)dec_part);

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

int
DS18B20_ReadAndSendDma(void)
{
    DS18B20_Data data;
    int rc = DS18B20_Read(&data);

    if(rc != 0)
    {
        return rc;
    }

    if(DS18B20_SendDma(&data) != HAL_OK)
    {
        return -4;
    }

    return 0;
}
