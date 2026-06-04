/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "KeyScan.h"
#include "LCD12864.h"
#include "btn.h"
#include "ch451.h"
#include "dc_motor.h"
#include "dht11.h"
#include "ds18b20.h"
#include "ir_receiver.h"
#include "led.h"
#include "servo_motor.h"
#include "spi.h"
#include "stepper_motor.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BTN_DEBOUNCE_DELAY 40 // 按键消抖时间，单位 ms
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for BtnTask */
osThreadId_t BtnTaskHandle;
const osThreadAttr_t BtnTask_attributes = {
    .name = "BtnTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for SwitchTask */
osThreadId_t SwitchTaskHandle;
const osThreadAttr_t SwitchTask_attributes = {
    .name = "SwitchTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for ADCTask */
osThreadId_t ADCTaskHandle;
const osThreadAttr_t ADCTask_attributes = {
    .name = "ADCTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for UARTTask */
osThreadId_t UARTTaskHandle;
const osThreadAttr_t UARTTask_attributes = {
    .name = "UARTTask",
    .stack_size = 384 * 4,
    .priority = (osPriority_t)osPriorityAboveNormal,
};
/* Definitions for TrafficTask */
osThreadId_t TrafficTaskHandle;
const osThreadAttr_t TrafficTask_attributes = {
    .name = "TrafficTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
    .name = "DisplayTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for SensorTask */
osThreadId_t SensorTaskHandle;
const osThreadAttr_t SensorTask_attributes = {
    .name = "SensorTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityHigh,
};
/* Definitions for MotorTask */
osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
    .name = "MotorTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for InfraredTask */
osThreadId_t InfraredTaskHandle;
const osThreadAttr_t InfraredTask_attributes = {
    .name = "InfraredTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for DACTask */
osThreadId_t DACTaskHandle;
const osThreadAttr_t DACTask_attributes = {
    .name = "DACTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for CH451Task */
osThreadId_t CH451TaskHandle;
const osThreadAttr_t CH451Task_attributes = {
    .name = "CH451Task",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for DIP2LCDQueue */
osMessageQueueId_t DIP2LCDQueueHandle;
const osMessageQueueAttr_t DIP2LCDQueue_attributes = { .name = "DIP2LCDQueue" };
/* Definitions for UARTTXQueue */
osMessageQueueId_t UARTTXQueueHandle;
const osMessageQueueAttr_t UARTTXQueue_attributes = { .name = "UARTTXQueue" };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartBtnTask(void *argument);
void StartSwitchTask(void *argument);
void StartADCTask(void *argument);
void StartUARTTask(void *argument);
void StartTrafficTask(void *argument);
void StartDisplayTask(void *argument);
void StartSensorTask(void *argument);
void StartMotorTask(void *argument);
void StartInfraredTask(void *argument);
void StartDACTask(void *argument);
void StartCH451Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void
MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the queue(s) */
    /* creation of DIP2LCDQueue */
    DIP2LCDQueueHandle = osMessageQueueNew(2, sizeof(uint8_t), &DIP2LCDQueue_attributes);

    /* creation of UARTTXQueue */
    UARTTXQueueHandle = osMessageQueueNew(8, sizeof(UartMsg_t), &UARTTXQueue_attributes);

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of BtnTask */
    BtnTaskHandle = osThreadNew(StartBtnTask, NULL, &BtnTask_attributes);

    /* creation of SwitchTask */
    SwitchTaskHandle = osThreadNew(StartSwitchTask, NULL, &SwitchTask_attributes);

    /* creation of ADCTask */
    ADCTaskHandle = osThreadNew(StartADCTask, NULL, &ADCTask_attributes);

    /* creation of UARTTask */
    UARTTaskHandle = osThreadNew(StartUARTTask, NULL, &UARTTask_attributes);

    /* creation of TrafficTask */
    TrafficTaskHandle = osThreadNew(StartTrafficTask, NULL, &TrafficTask_attributes);

    /* creation of DisplayTask */
    DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

    /* creation of SensorTask */
    SensorTaskHandle = osThreadNew(StartSensorTask, NULL, &SensorTask_attributes);

    /* creation of MotorTask */
    MotorTaskHandle = osThreadNew(StartMotorTask, NULL, &MotorTask_attributes);

    /* creation of InfraredTask */
    InfraredTaskHandle = osThreadNew(StartInfraredTask, NULL, &InfraredTask_attributes);

    /* creation of DACTask */
    DACTaskHandle = osThreadNew(StartDACTask, NULL, &DACTask_attributes);

    /* creation of CH451Task */
    CH451TaskHandle = osThreadNew(StartCH451Task, NULL, &CH451Task_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartBtnTask */
/**
 * @brief  Function implementing the BtnTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartBtnTask */
void
StartBtnTask(void *argument)
{
    /* USER CODE BEGIN StartBtnTask */
    uint8_t debounceCounter; // 消抖计数器
    /* Infinite loop */
    for(;;)
    {
        // 读取按键状态
        uint8_t btnPressed = HAL_GPIO_ReadPin(WKUP_GPIO_Port, WKUP_Pin) == GPIO_PIN_RESET;
        switch(btnState)
        {
            case BTN_STATE_IDLE:
                if(btnPressed)
                {
                    btnState = BTN_STATE_DEBOUNCE;
                    debounceCounter = 0;
                }
                break;

            case BTN_STATE_DEBOUNCE:
                if(btnPressed)
                {
                    debounceCounter++;
                    if(debounceCounter >= BTN_DEBOUNCE_DELAY / 20)
                    { // 每 20 ms 检查一次
                        btnState = BTN_STATE_PRESSED;
                    }
                }
                else
                {
                    btnState = BTN_STATE_IDLE; // 按键抖动，回到空闲状态
                }
                break;

            case BTN_STATE_PRESSED:
                if(btnPressed)
                {
                    break;
                }

                btnEvent = BTN_EVENT_CLICK; // 点击事件
                btnState = BTN_STATE_IDLE;  // 回到空闲状态
                break;
        }

        KeyScan_ScanAll(); // 检测四个独立按键的状态

        osDelay(20);
    }
    /* USER CODE END StartBtnTask */
}

/* USER CODE BEGIN Header_StartSwitchTask */
/**
 * @brief Function implementing the SwitchTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartSwitchTask */
void
StartSwitchTask(void *argument)
{
    /* USER CODE BEGIN StartSwitchTask */
    /* Infinite loop */
    for(;;)
    {
        // 读取 DIP 开关状态，发送到 DisplayTask 显示
        uint8_t dipState = 0;
        dipState |= (HAL_GPIO_ReadPin(DIP0_GPIO_Port, DIP0_Pin) == GPIO_PIN_RESET) ? 0x01 : 0x00;
        dipState |= (HAL_GPIO_ReadPin(DIP1_GPIO_Port, DIP1_Pin) == GPIO_PIN_RESET) ? 0x02 : 0x00;
        dipState |= (HAL_GPIO_ReadPin(DIP2_GPIO_Port, DIP2_Pin) == GPIO_PIN_RESET) ? 0x04 : 0x00;
        dipState |= (HAL_GPIO_ReadPin(DIP3_GPIO_Port, DIP3_Pin) == GPIO_PIN_RESET) ? 0x08 : 0x00;
        dipState |= (HAL_GPIO_ReadPin(DIP4_GPIO_Port, DIP4_Pin) == GPIO_PIN_RESET) ? 0x10 : 0x00;
        dipState |= (HAL_GPIO_ReadPin(DIP5_GPIO_Port, DIP5_Pin) == GPIO_PIN_RESET) ? 0x20 : 0x00;
        dipState |= (HAL_GPIO_ReadPin(DIP6_GPIO_Port, DIP6_Pin) == GPIO_PIN_RESET) ? 0x40 : 0x00;
        dipState |= (HAL_GPIO_ReadPin(DIP7_GPIO_Port, DIP7_Pin) == GPIO_PIN_RESET) ? 0x80 : 0x00;
        osMessageQueuePut(DIP2LCDQueueHandle, &dipState, 0, 0);
        osDelay(200);

        // 独立按键 KEY3 控制设备控制模式的切换
        if(keyEvent[3] == KEY_EVENT_CLICK)
        {
            keyEvent[3] = KEY_EVENT_NONE; // 清除事件
            switch(deviceControlMode)
            {
                case RELAY_CONTROL:
                    deviceControlMode = STEPPER_MOTOR_CONTROL;
                    break;

                case STEPPER_MOTOR_CONTROL:
                    deviceControlMode = DC_MOTOR_CONTROL;
                    break;

                case DC_MOTOR_CONTROL:
                    deviceControlMode = SERVO_MOTOR_CONTROL;
                    break;

                case SERVO_MOTOR_CONTROL:
                    deviceControlMode = RELAY_CONTROL;
                    break;
            }

            // 切换设备控制模式时，电机关闭，预设模式重置
            motorStartStopState = MOTOR_OFF;
            motorMode = MOTOR_MODE1;
        }

        // 处于继电器控制模式时，独立按键 KEY0 和 KEY1 控制继电器 RELAY0 和 RELAY1 的开关
        if(deviceControlMode == RELAY_CONTROL)
        {
            // 根据独立按键 KEY0 和 KEY1 的状态，控制继电器 RELAY0 和 RELAY1 的开关
            if(keyEvent[0] == KEY_EVENT_CLICK)
            {
                keyEvent[0] = KEY_EVENT_NONE; // 清除事件
                HAL_GPIO_TogglePin(RELAY0_GPIO_Port, RELAY0_Pin);
            }
            if(keyEvent[1] == KEY_EVENT_CLICK)
            {
                keyEvent[1] = KEY_EVENT_NONE; // 清除事件
                HAL_GPIO_TogglePin(RELAY1_GPIO_Port, RELAY1_Pin);
            }
        }
        else
        {
            // 非继电器控制模式下，继电器断开
            HAL_GPIO_WritePin(RELAY0_GPIO_Port, RELAY0_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);
        }
        osDelay(100);
    }
    /* USER CODE END StartSwitchTask */
}

/* USER CODE BEGIN Header_StartADCTask */
/**
 * @brief Function implementing the ADCTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartADCTask */
void
StartADCTask(void *argument)
{
    /* USER CODE BEGIN StartADCTask */

    /* Infinite loop */
    for(;;)
    {
        // 根据按键事件切换显示模式
        if(btnEvent == BTN_EVENT_CLICK)
        {
            btnEvent = BTN_EVENT_NONE; // 清除事件
            if(LED_displayMode == LED_DISPLAY_LIGHT)
            {
                LED_displayMode = LED_DISPLAY_TEMP; // 切换到显示热敏电阻
            }
            else
            {
                LED_displayMode = LED_DISPLAY_LIGHT; // 切换到显示光敏电阻
            }
        }

        // 根据当前显示模式，切换 LED 显示光敏电阻的采样值还是热敏电阻的采样值
        if(LED_displayMode == LED_DISPLAY_LIGHT)
        {
            // 通过板载 LED 显示光敏电阻的采样值，adc_value[0]，范围 0~4095，点亮 LED0 到 LED11
            LED_Binary_Display(adc_value[0]);
        }
        else if(LED_displayMode == LED_DISPLAY_TEMP)
        {
            // 通过板载 LED 显示热敏电阻的采样值，adc_value[1]，范围 0~4095，点亮 LED0 到 LED11
            LED_Binary_Display(adc_value[1]);
        }
        osDelay(200);
    }
    /* USER CODE END StartADCTask */
}

/* USER CODE BEGIN Header_StartUARTTask */
/**
 * @brief Function implementing the UARTTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartUARTTask */
void
StartUARTTask(void *argument)
{
    /* USER CODE BEGIN StartUARTTask */
    UartMsg_t msg;
    /* Infinite loop */
    for(;;)
    {
        // 从 UART 接收队列获取数据，并通过 UART 发送出去
        if(osMessageQueueGet(UARTTXQueueHandle, &msg, NULL, osWaitForever) == osOK)
        {
            if(msg.len > 0)
            {
                if(msg.len > UART_TX_MAX_LEN)
                {
                    msg.len = UART_TX_MAX_LEN;
                }

                while(HAL_UART_Transmit_DMA(&huart1, msg.data, msg.len) != HAL_OK)
                {
                    osDelay(10);
                }

                while(huart1.gState != HAL_UART_STATE_READY)
                {
                    osDelay(10);
                }
            }
        }
        osDelay(100);
    }
    /* USER CODE END StartUARTTask */
}

/* USER CODE BEGIN Header_StartTrafficTask */
/**
 * @brief Function implementing the TrafficTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTrafficTask */
void
StartTrafficTask(void *argument)
{
    /* USER CODE BEGIN StartTrafficTask */
    /* Infinite loop */
    for(;;)
    {
        // 根据 trafficLightState 的状态，控制交通灯的 LED 显示(低电平点亮 LED)
        switch(trafficLightState)
        {
            case NS_GREEN_WE_RED:
                HAL_GPIO_WritePin(NS_G_GPIO_Port, NS_G_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(NS_Y_GPIO_Port, NS_Y_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(NS_R_GPIO_Port, NS_R_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(WE_G_GPIO_Port, WE_G_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(WE_Y_GPIO_Port, WE_Y_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(WE_R_GPIO_Port, WE_R_Pin, GPIO_PIN_RESET);
                break;

            case NS_YELLOW_WE_RED:
                HAL_GPIO_WritePin(NS_G_GPIO_Port, NS_G_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(NS_Y_GPIO_Port, NS_Y_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(NS_R_GPIO_Port, NS_R_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(WE_G_GPIO_Port, WE_G_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(WE_Y_GPIO_Port, WE_Y_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(WE_R_GPIO_Port, WE_R_Pin, GPIO_PIN_RESET);
                break;

            case NS_RED_WE_GREEN:
                HAL_GPIO_WritePin(NS_G_GPIO_Port, NS_G_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(NS_Y_GPIO_Port, NS_Y_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(NS_R_GPIO_Port, NS_R_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(WE_G_GPIO_Port, WE_G_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(WE_Y_GPIO_Port, WE_Y_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(WE_R_GPIO_Port, WE_R_Pin, GPIO_PIN_SET);
                break;

            case NS_RED_WE_YELLOW:
                HAL_GPIO_WritePin(NS_G_GPIO_Port, NS_G_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(NS_Y_GPIO_Port, NS_Y_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(NS_R_GPIO_Port, NS_R_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(WE_G_GPIO_Port, WE_G_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(WE_Y_GPIO_Port, WE_Y_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(WE_R_GPIO_Port, WE_R_Pin, GPIO_PIN_SET);
                break;
        }
        osDelay(1000);
    }
    /* USER CODE END StartTrafficTask */
}

/* USER CODE BEGIN Header_StartDisplayTask */
/**
 * @brief Function implementing the DisplayTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDisplayTask */
void
StartDisplayTask(void *argument)
{
    /* USER CODE BEGIN StartDisplayTask */
    // 显存
    char row1[17] = { 0 };
    char row2[17] = { 0 };
    char row3[17] = { 0 };
    char row4[17] = { 0 };
    char row1Text[32] = { 0 };
    char row4Text[32] = { 0 };

    const char *trafficText = "NS:Green WE:Red  ";
    const char *deviceControlText[] = { "Relay Ctrl", "Stepper   ", "DC Motor  ", "Servo     " };
    /* Infinite loop */
    for(;;)
    {
        // 在 LCD 上显示当前的设备控制模式；非继电器模式时追加开关状态
        if(deviceControlMode == RELAY_CONTROL)
        {
            snprintf(row1Text, sizeof(row1Text), "%s", deviceControlText[deviceControlMode]);
        }
        else
        {
            if(motorStartStopState == MOTOR_ON)
            {
                snprintf(row1Text,
                         sizeof(row1Text),
                         "%s %u ON ",
                         deviceControlText[deviceControlMode],
                         (unsigned int)motorMode + 1U);
            }
            else
            {
                snprintf(row1Text,
                         sizeof(row1Text),
                         "%s %u OFF",
                         deviceControlText[deviceControlMode],
                         (unsigned int)motorMode + 1U);
            }
        }
        memset(row1, ' ', 16);
        memcpy(row1, row1Text, strlen(row1Text) > 16 ? 16 : strlen(row1Text));
        row1[16] = '\0';
        LCD_DispString(ROW1, COL1, row1);

        // 在 LCD 上显示当前的交通灯状态，显示在第二行
        switch(trafficLightState)
        {
            case NS_GREEN_WE_RED:
                trafficText = "NS:Green WE:Red  ";
                break;

            case NS_YELLOW_WE_RED:
                trafficText = "NS:Yellow WE:Red ";
                break;

            case NS_RED_WE_GREEN:
                trafficText = "NS:Red WE:Green  ";
                break;

            case NS_RED_WE_YELLOW:
                trafficText = "NS:Red WE:Yellow ";
                break;
        }

        memcpy(row2, trafficText, 16);
        row2[16] = '\0';
        LCD_DispString(ROW2, COL1, row2);

        // 从消息队列接收 DIP 开关状态，显示在第三行，每次显示两位
        uint8_t dipState;
        osMessageQueueGet(DIP2LCDQueueHandle, &dipState, 0, 0);
        memcpy(row3, "DIP:    ", 8);
        for(int i = 0; i < 4; i++)
        {
            // 每次提取两位：先高位后低位
            uint8_t twoBits = (dipState >> (6 - 2 * i)) & 0x03;
            row3[8 + 2 * i] = (char)('0' + ((twoBits >> 1) & 0x01));
            row3[8 + 2 * i + 1] = (char)('0' + (twoBits & 0x01));
        }
        row3[16] = '\0';
        LCD_DispString(ROW3, COL1, row3);

        // 按键 2 暂时无用，先用于控制 LED 14 亮灭
        if(keyEvent[2] == KEY_EVENT_CLICK)
        {
            keyEvent[2] = KEY_EVENT_NONE; // 清除事件
            HAL_GPIO_TogglePin(LED14_GPIO_Port, LED14_Pin);
        }

        // 第四行内容待改
        snprintf(row4Text, sizeof(row4Text), "%s", "Enter text");
        memset(row4, ' ', 16);
        memcpy(row4, row4Text, strlen(row4Text) > 16 ? 16 : strlen(row4Text));
        row4[16] = '\0';
        LCD_DispString(ROW4, COL1, row4);

        osDelay(100);
    }
    /* USER CODE END StartDisplayTask */
}

/* USER CODE BEGIN Header_StartSensorTask */
/**
 * @brief Function implementing the SensorTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartSensorTask */
void
StartSensorTask(void *argument)
{
    /* USER CODE BEGIN StartSensorTask */
    // 初始化传感器
    DHT11_Init();   // 初始化 DHT11 温湿度传感器
    DS18B20_Init(); // 初始化 DS18B20 温度传感器

    /* Infinite loop */
    for(;;)
    {
        // 读取 DHT11 和 DS18B20 传感器数据，并通过 UART 发送队列发送到上位机
        DHT11_Data dht11_data;
        if(DHT11_Read(&dht11_data) == 0)
        {
            UartMsg_t msg;
            int len = snprintf((char *)msg.data,
                               sizeof(msg.data),
                               "DHT11 H:%u.%u T:%u.%u\r\n",
                               (unsigned int)dht11_data.humidity_int,
                               (unsigned int)dht11_data.humidity_dec,
                               (unsigned int)dht11_data.temperature_int,
                               (unsigned int)dht11_data.temperature_dec);

            if(len > 0)
            {
                if(len > UART_TX_MAX_LEN)
                {
                    len = UART_TX_MAX_LEN;
                }
                msg.len = (uint16_t)len;
                (void)osMessageQueuePut(UARTTXQueueHandle, &msg, 0, 0);
            }
        }

        DS18B20_Data ds18b20_data;
        if(DS18B20_Read(&ds18b20_data) == 0)
        {
            UartMsg_t msg;
            int16_t temp_x10 = ds18b20_data.temperature_x10;
            int16_t abs_x10 = (temp_x10 < 0) ? (int16_t)(-temp_x10) : temp_x10;
            int16_t int_part = (int16_t)(abs_x10 / 10);
            int16_t dec_part = (int16_t)(abs_x10 % 10);
            char sign = (temp_x10 < 0) ? '-' : '+';
            int len = snprintf((char *)msg.data,
                               sizeof(msg.data),
                               "DS18B20 T:%c%d.%d\r\n",
                               sign,
                               (int)int_part,
                               (int)dec_part);

            if(len > 0)
            {
                if(len > UART_TX_MAX_LEN)
                {
                    len = UART_TX_MAX_LEN;
                }
                msg.len = (uint16_t)len;
                (void)osMessageQueuePut(UARTTXQueueHandle, &msg, 0, 0);
            }
        }

        osDelay(4000); // 每 4 秒读取一次传感器数据
    }
    /* USER CODE END StartSensorTask */
}

/* USER CODE BEGIN Header_StartMotorTask */
/**
 * @brief Function implementing the MotorTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMotorTask */
void
StartMotorTask(void *argument)
{
    /* USER CODE BEGIN StartMotorTask */
    Stepper_Init();    // 初始化步进电机
    DCMotor_Init();    // 初始化直流电机
    ServoMotor_Init(); // 初始化舵机

    /* Infinite loop */
    for(;;)
    {
        // KEY0 用来控制步进电机 / 直流电机 / 舵机开关，KEY1 用来控制步进电机 / 直流电机 /
        // 舵机状态切换
        if(deviceControlMode != RELAY_CONTROL)
        {
            if(keyEvent[0] == KEY_EVENT_CLICK)
            {
                keyEvent[0] = KEY_EVENT_NONE; // 清除事件
                motorStartStopState =
                  (motorStartStopState == MOTOR_OFF) ? MOTOR_ON : MOTOR_OFF; // 切换电机开关状态
            }
            if(keyEvent[1] == KEY_EVENT_CLICK)
            {
                keyEvent[1] = KEY_EVENT_NONE; // 清除事件
                motorMode =
                  (motorMode == MOTOR_MODE1) ? MOTOR_MODE2 : MOTOR_MODE1; // 切换电机预设模式
            }
        }

        // 根据当前的设备控制模式和电机状态，控制对应设备的运行
        switch(deviceControlMode)
        {
            // 步进电机
            case STEPPER_MOTOR_CONTROL:
                if(motorStartStopState == MOTOR_ON)
                {
                    switch(motorMode)
                    {
                        case MOTOR_MODE1:
                            Stepper_RotateAngle(90.0f, 1000); // 正转 90 度
                            osDelay(200);                     // 运行一段时间后再切换状态
                            break;
                        case MOTOR_MODE2:
                            Stepper_RotateAngle(-90.0f, 1000); // 反转 90 度
                            osDelay(200);                      // 运行一段时间后再切换状态
                            break;
                    }
                }
                else
                {
                    Stepper_Stop(); // 步进电机关闭时停止输出
                }

                DCMotor_Brake();   // 其他模式时直流电机制动
                ServoMotor_Home(); // 其他模式时舵机归零
                break;
            // 直流电机
            case DC_MOTOR_CONTROL:
                if(motorStartStopState == MOTOR_ON)
                {
                    switch(motorMode)
                    {
                        case MOTOR_MODE1:
                            DCMotor_Forward(DCMOTOR_DEFAULT_DUTY_PERCENT);
                            break;

                        case MOTOR_MODE2:
                            DCMotor_Reverse(DCMOTOR_DEFAULT_DUTY_PERCENT);
                            break;
                    }
                }
                else
                {
                    DCMotor_Brake(); // 直流电机关闭时制动
                }

                Stepper_Stop();    // 其他模式时步进电机停止
                ServoMotor_Home(); // 其他模式时舵机归零
                break;
            // 舵机
            case SERVO_MOTOR_CONTROL:
                if(motorStartStopState == MOTOR_ON)
                {
                    switch(motorMode)
                    {
                        case MOTOR_MODE1:
                            ServoMotor_Mode1();
                            break;
                        case MOTOR_MODE2:
                            ServoMotor_Mode2();
                            break;
                    }
                }
                else
                {
                    ServoMotor_Home(); // 舵机关闭时归零
                }

                Stepper_Stop();  // 其他模式时步进电机停止
                DCMotor_Brake(); // 其他模式时直流电机制动
                break;
            default:
                Stepper_Stop();    // 其他模式时步进电机停止
                DCMotor_Brake();   // 其他模式时直流电机制动
                ServoMotor_Home(); // 其他模式时舵机归零
                break;
        }
        osDelay(10);
    }
    /* USER CODE END StartMotorTask */
}

/* USER CODE BEGIN Header_StartInfraredTask */
/**
 * @brief Function implementing the InfraredTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartInfraredTask */
void
StartInfraredTask(void *argument)
{
    /* USER CODE BEGIN StartInfraredTask */
    /* Infinite loop */
    for(;;)
    {
        // 读取红外接收器状态，并通过串口输出按键值
        if(IR_Is_Data_Ready())
        {
            // 重复码不发送，只发送首次按键和不同按键的切换
            if(!IR_Is_Repeat())
            {
                uint8_t cmd = IR_Get_Command();
                const char *irKey = IR_Get_Key();

                // ── 红外遥控控制逻辑 ──────────────────────────────
                if(cmd == 0x40) // NEXT: 向后切换设备控制模式
                {
                    switch(deviceControlMode)
                    {
                        case RELAY_CONTROL:
                            deviceControlMode = STEPPER_MOTOR_CONTROL;
                            break;
                        case STEPPER_MOTOR_CONTROL:
                            deviceControlMode = DC_MOTOR_CONTROL;
                            break;
                        case DC_MOTOR_CONTROL:
                            deviceControlMode = SERVO_MOTOR_CONTROL;
                            break;
                        case SERVO_MOTOR_CONTROL:
                            deviceControlMode = RELAY_CONTROL;
                            break;
                    }
                    // 切换设备控制模式时，电机关闭，预设模式重置
                    motorStartStopState = MOTOR_OFF;
                    motorMode = MOTOR_MODE1;
                }
                else if(cmd == 0x44) // PREV: 向前切换设备控制模式
                {
                    switch(deviceControlMode)
                    {
                        case RELAY_CONTROL:
                            deviceControlMode = SERVO_MOTOR_CONTROL;
                            break;
                        case SERVO_MOTOR_CONTROL:
                            deviceControlMode = DC_MOTOR_CONTROL;
                            break;
                        case DC_MOTOR_CONTROL:
                            deviceControlMode = STEPPER_MOTOR_CONTROL;
                            break;
                        case STEPPER_MOTOR_CONTROL:
                            deviceControlMode = RELAY_CONTROL;
                            break;
                    }
                    // 切换设备控制模式时，电机关闭，预设模式重置
                    motorStartStopState = MOTOR_OFF;
                    motorMode = MOTOR_MODE1;
                }
                else if(cmd == 0x45) // CH-: 继电器模式控制 RELAY0；非继电器模式切换预设模式
                {
                    if(deviceControlMode == RELAY_CONTROL)
                    {
                        HAL_GPIO_TogglePin(RELAY0_GPIO_Port, RELAY0_Pin);
                    }
                    else
                    {
                        motorMode = (motorMode == MOTOR_MODE1) ? MOTOR_MODE2 : MOTOR_MODE1;
                    }
                }
                else if(cmd == 0x47) // CH+: 继电器模式控制 RELAY1；非继电器模式切换预设模式
                {
                    if(deviceControlMode == RELAY_CONTROL)
                    {
                        HAL_GPIO_TogglePin(RELAY1_GPIO_Port, RELAY1_Pin);
                    }
                    else
                    {
                        motorMode = (motorMode == MOTOR_MODE1) ? MOTOR_MODE2 : MOTOR_MODE1;
                    }
                }
                else if(cmd == 0x43) // PLAY/PAUSE: 非继电器模式下控制电机开关
                {
                    if(deviceControlMode != RELAY_CONTROL)
                    {
                        motorStartStopState =
                          (motorStartStopState == MOTOR_OFF) ? MOTOR_ON : MOTOR_OFF;
                    }
                }

                // 通过串口输出按键值
                UartMsg_t msg;
                int len = snprintf((char *)msg.data, sizeof(msg.data), "IR Key: %s\r\n", irKey);
                if(len > 0)
                {
                    if(len > UART_TX_MAX_LEN)
                    {
                        len = UART_TX_MAX_LEN;
                    }
                    msg.len = (uint16_t)len;
                    (void)osMessageQueuePut(UARTTXQueueHandle, &msg, 0, 0);
                }
            }
            // 无论是否发送，都需清除数据就绪标志以接收下一帧
            IR_Clear_Data_Ready();
        }
        osDelay(100);
    }
    /* USER CODE END StartInfraredTask */
}

/* USER CODE BEGIN Header_StartDACTask */
/**
 * @brief Function implementing the DACTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDACTask */
void
StartDACTask(void *argument)
{
    /* USER CODE BEGIN StartDACTask */
    uint16_t dacValue = 0; // DAC 输出值，范围 0~1023

    /* Infinite loop */
    for(;;)
    {
        // 将 dacValue 转换为 12 位值，输出到 TLC5615
        uint16_t tlcValue = (dacValue & 0x03FF) << 2; // 左移 2 位，适配 TLC5615 的 12 位输入
        // 通过 SPI DMA 发送 tlcValue 到 TLC5615
        HAL_GPIO_WritePin(TLC5615_CS_GPIO_Port, TLC5615_CS_Pin, GPIO_PIN_RESET); // 片选拉低
        HAL_SPI_Transmit_DMA(&hspi3, (uint8_t *)&tlcValue, 1);
        HAL_GPIO_WritePin(TLC5615_CS_GPIO_Port, TLC5615_CS_Pin, GPIO_PIN_SET); // 片选拉高

        // 改变 dacValue 的值，实现呼吸灯效果
        dacValue += 25;
        if(dacValue > 500)
        {
            dacValue = 0;
        }
        osDelay(100);
    }
    /* USER CODE END StartDACTask */
}

/* USER CODE BEGIN Header_StartCH451Task */
/**
 * @brief Function implementing the CH451Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartCH451Task */
void
StartCH451Task(void *argument)
{
    /* USER CODE BEGIN StartCH451Task */
    CH451_Init();
    CH451_ClearDisplay();

    /* Infinite loop */
    for(;;)
    {
        if(CH451_KeyPressed())
        {
            uint8_t keycode = CH451_ReadKeyCode();
            uint8_t row = CH451_GetKeyRow(keycode);
            uint8_t col = CH451_GetKeyCol(keycode);
            uint8_t pressed = CH451_IsKeyPress(keycode);
            CH451_DisplayKeyMatrix(row, col, pressed);
        }
        osDelay(20);
    }
    /* USER CODE END StartCH451Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
