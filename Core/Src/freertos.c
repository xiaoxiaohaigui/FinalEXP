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
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LCD12864.h"
#include "btn.h"
#include "led.h"

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
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LEDTask */
osThreadId_t LEDTaskHandle;
const osThreadAttr_t LEDTask_attributes = {
  .name = "LEDTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for ADCTask */
osThreadId_t ADCTaskHandle;
const osThreadAttr_t ADCTask_attributes = {
  .name = "ADCTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UARTTask */
osThreadId_t UARTTaskHandle;
const osThreadAttr_t UARTTask_attributes = {
  .name = "UARTTask",
  .stack_size = 384 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for TrafficTask */
osThreadId_t TrafficTaskHandle;
const osThreadAttr_t TrafficTask_attributes = {
  .name = "TrafficTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SensorTask */
osThreadId_t SensorTaskHandle;
const osThreadAttr_t SensorTask_attributes = {
  .name = "SensorTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for MotorTask */
osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
  .name = "MotorTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for InfraredTask */
osThreadId_t InfraredTaskHandle;
const osThreadAttr_t InfraredTask_attributes = {
  .name = "InfraredTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DACTask */
osThreadId_t DACTaskHandle;
const osThreadAttr_t DACTask_attributes = {
  .name = "DACTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartBtnTask(void *argument);
void StartLEDTask(void *argument);
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
void MX_FREERTOS_Init(void) {
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

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of BtnTask */
  BtnTaskHandle = osThreadNew(StartBtnTask, NULL, &BtnTask_attributes);

  /* creation of LEDTask */
  LEDTaskHandle = osThreadNew(StartLEDTask, NULL, &LEDTask_attributes);

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
void StartBtnTask(void *argument)
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
        osDelay(20);
    }
  /* USER CODE END StartBtnTask */
}

/* USER CODE BEGIN Header_StartLEDTask */
/**
 * @brief Function implementing the LEDTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLEDTask */
void StartLEDTask(void *argument)
{
  /* USER CODE BEGIN StartLEDTask */
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
    }
  /* USER CODE END StartLEDTask */
}

/* USER CODE BEGIN Header_StartADCTask */
/**
 * @brief Function implementing the ADCTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartADCTask */
void StartADCTask(void *argument)
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
void StartUARTTask(void *argument)
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
void StartTrafficTask(void *argument)
{
  /* USER CODE BEGIN StartTrafficTask */
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
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
void StartDisplayTask(void *argument)
{
  /* USER CODE BEGIN StartDisplayTask */
    LCD_DispString(ROW1, COL1, "Hello, FreeRTOS!");
    /* Infinite loop */
    for(;;)
    {

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
void StartSensorTask(void *argument)
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
void StartMotorTask(void *argument)
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
void StartInfraredTask(void *argument)
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
void StartDACTask(void *argument)
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

