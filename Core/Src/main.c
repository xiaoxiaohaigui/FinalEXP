/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "cmsis_os.h"
#include "dma.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DelayUs.h"
#include "KeyScan.h"
#include "LCD12864.h"
#include "btn.h"
#include "dc_motor.h"
#include "ir_receiver.h"
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint16_t
  adc_value[2]; // 存放 ADC 值，adc_value[0] 存放光敏电阻的采样值，adc_value[1] 存放热敏电阻的采样值
uint8_t rxData[50]; // 接收缓冲区（用于回显）

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
extern osMessageQueueId_t UARTTXQueueHandle;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
LED_DisplayMode_t LED_displayMode = LED_DISPLAY_LIGHT;          // 默认显示光敏电阻的采样值
TrafficLightState_t trafficLightState = NS_GREEN_WE_RED;        // 初始化交通灯状态为南北绿，东西红
volatile DeviceControlMode_t deviceControlMode = RELAY_CONTROL; // 初始化设备控制模式为继电器控制
volatile MotorStartStopState_t motorStartStopState = MOTOR_OFF; // 初始化电机开关状态为关闭
volatile MotorMode_t motorMode = MOTOR_MODE1;                   // 初始化电机预设模式
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int
main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_TIM3_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();
    MX_TIM5_Init();
    MX_SPI3_Init();
    MX_TIM8_Init();
    MX_TIM4_Init();
    /* USER CODE BEGIN 2 */
    // 初始化按键
    Btn_Init();
    KeyScan_Init();

    // 初始化 LCD12864
    Delay_Init();
    LCD_Init();
    LCD_Clear();
    IR_Receiver_Init(); // 初始化红外接收器

    // 启动 ADC DMA 采集，连续采集光敏电阻和热敏电阻的模拟值，并存放到 adc_value 数组中
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, 2);

    // 启动 TIM5 中断，用于每 3 秒切换一次交通灯状态
    HAL_TIM_Base_Start_IT(&htim5);

    // 启动 UART 接收，使用空闲中断模式，接收数据存放到 rxData 数组中
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxData, sizeof(rxData));

    // 关闭 DMA 传输过半中断
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
    /* USER CODE END 2 */

    /* Init scheduler */
    osKernelInitialize(); /* Call init function for freertos objects (in cmsis_os2.c) */
    MX_FREERTOS_Init();

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void
SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void
HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart->Instance == USART1)
    {
        // 回显接收到的数据（通过 UART 发送队列）
        if(UARTTXQueueHandle != NULL)
        {
            UartMsg_t msg;
            uint16_t copy_len = Size;

            if(copy_len > UART_TX_MAX_LEN)
            {
                copy_len = UART_TX_MAX_LEN;
            }

            msg.len = copy_len;
            memcpy(msg.data, rxData, copy_len);
            (void)osMessageQueuePut(UARTTXQueueHandle, &msg, 0, 0);
        }
        // 重新启动 UART 接收
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxData, sizeof(rxData));
    }
}
/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void
HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */
    if(htim->Instance == TIM5)
    {
        // 每 3 秒切换一次交通灯状态
        switch(trafficLightState)
        {
            case NS_GREEN_WE_RED:
                trafficLightState = NS_YELLOW_WE_RED;
                break;

            case NS_YELLOW_WE_RED:
                trafficLightState = NS_RED_WE_GREEN;
                break;

            case NS_RED_WE_GREEN:
                trafficLightState = NS_RED_WE_YELLOW;
                break;

            case NS_RED_WE_YELLOW:
                trafficLightState = NS_GREEN_WE_RED;
                break;
        }
    }
    /* USER CODE END Callback 0 */
    if(htim->Instance == TIM6)
    {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void
Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while(1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void
assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
