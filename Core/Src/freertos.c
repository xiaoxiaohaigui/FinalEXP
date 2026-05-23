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
#include "led.h"
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BTN_DEBOUNCE_DELAY 40 // 按键消抖时间，单位ms
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
/* Definitions for DIP2LCDQueue */
osMessageQueueId_t DIP2LCDQueueHandle;
const osMessageQueueAttr_t DIP2LCDQueue_attributes = { .name = "DIP2LCDQueue" };

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
                    { // 每20ms检查一次
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
        // 读取DIP开关状态，发送到DisplayTask显示
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

        // 根据独立按键KEY0和KEY1的状态，控制继电器RELAY0和RELAY1的开关
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

        // 根据当前显示模式，切换LED显示光敏电阻的采样值还是热敏电阻的采样值
        if(LED_displayMode == LED_DISPLAY_LIGHT)
        {
            // 通过板载LED显示光敏电阻的采样值，adc_value[0]，范围0~4095，点亮LED0到LED11
            LED_Binary_Display(adc_value[0]);
        }
        else if(LED_displayMode == LED_DISPLAY_TEMP)
        {
            // 通过板载LED显示热敏电阻的采样值，adc_value[1]，范围0~4095，点亮LED0到LED11
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

    /* Infinite loop */
    for(;;)
    {
        osDelay(50);
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
        // 根据trafficLightState的状态，控制交通灯的LED显示(低电平点亮LED)
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
    LCD_DispString(ROW1, COL1, "Hello, FreeRTOS!");

    char row2[17] = { 0 };
    char row3[17] = { 0 };
    const char *trafficText = "NS:Green WE:Red  ";
    /* Infinite loop */
    for(;;)
    {
        // 在LCD上显示当前的交通灯状态，显示在第二行
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

        // 从消息队列接收DIP开关状态，显示在第三行，每次显示两位
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

        // 根据独立按键状态控制LED的开关
        if(keyEvent[2] == KEY_EVENT_CLICK)
        {
            keyEvent[2] = KEY_EVENT_NONE; // 清除事件
            HAL_GPIO_TogglePin(LED14_GPIO_Port, LED14_Pin);
        }
        if(keyEvent[3] == KEY_EVENT_CLICK)
        {
            keyEvent[3] = KEY_EVENT_NONE; // 清除事件
            HAL_GPIO_TogglePin(LED15_GPIO_Port, LED15_Pin);
        }

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
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
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
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
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
        osDelay(1);
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
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
    }
    /* USER CODE END StartDACTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
