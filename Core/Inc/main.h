/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOF
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOF
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOF
#define LED4_Pin GPIO_PIN_4
#define LED4_GPIO_Port GPIOF
#define LED5_Pin GPIO_PIN_5
#define LED5_GPIO_Port GPIOF
#define LED6_Pin GPIO_PIN_6
#define LED6_GPIO_Port GPIOF
#define LED7_Pin GPIO_PIN_7
#define LED7_GPIO_Port GPIOF
#define LED8_Pin GPIO_PIN_8
#define LED8_GPIO_Port GPIOF
#define LED9_Pin GPIO_PIN_9
#define LED9_GPIO_Port GPIOF
#define LED10_Pin GPIO_PIN_10
#define LED10_GPIO_Port GPIOF
#define WKUP_Pin GPIO_PIN_0
#define WKUP_GPIO_Port GPIOA
#define ADC_LIGHT_Pin GPIO_PIN_1
#define ADC_LIGHT_GPIO_Port GPIOA
#define ADC_TEMP_Pin GPIO_PIN_2
#define ADC_TEMP_GPIO_Port GPIOA
#define LCD12864_CS_Pin GPIO_PIN_3
#define LCD12864_CS_GPIO_Port GPIOA
#define LCD12864_MOSI_Pin GPIO_PIN_5
#define LCD12864_MOSI_GPIO_Port GPIOA
#define LCD12864_CLK_Pin GPIO_PIN_6
#define LCD12864_CLK_GPIO_Port GPIOA
#define LCD12864_RST_Pin GPIO_PIN_7
#define LCD12864_RST_GPIO_Port GPIOA
#define DHT11_Pin GPIO_PIN_4
#define DHT11_GPIO_Port GPIOC
#define DS18B20_Pin GPIO_PIN_5
#define DS18B20_GPIO_Port GPIOC
#define RELAY0_Pin GPIO_PIN_0
#define RELAY0_GPIO_Port GPIOB
#define RELAY1_Pin GPIO_PIN_1
#define RELAY1_GPIO_Port GPIOB
#define INFRARED_Pin GPIO_PIN_2
#define INFRARED_GPIO_Port GPIOB
#define LED11_Pin GPIO_PIN_11
#define LED11_GPIO_Port GPIOF
#define LED12_Pin GPIO_PIN_12
#define LED12_GPIO_Port GPIOF
#define LED13_Pin GPIO_PIN_13
#define LED13_GPIO_Port GPIOF
#define LED14_Pin GPIO_PIN_14
#define LED14_GPIO_Port GPIOF
#define LED15_Pin GPIO_PIN_15
#define LED15_GPIO_Port GPIOF
#define KEY0_Pin GPIO_PIN_1
#define KEY0_GPIO_Port GPIOG
#define KEY1_Pin GPIO_PIN_7
#define KEY1_GPIO_Port GPIOE
#define KEY2_Pin GPIO_PIN_8
#define KEY2_GPIO_Port GPIOE
#define KEY3_Pin GPIO_PIN_9
#define KEY3_GPIO_Port GPIOE
#define DIP0_Pin GPIO_PIN_10
#define DIP0_GPIO_Port GPIOE
#define DIP1_Pin GPIO_PIN_11
#define DIP1_GPIO_Port GPIOE
#define DIP2_Pin GPIO_PIN_12
#define DIP2_GPIO_Port GPIOE
#define DIP3_Pin GPIO_PIN_13
#define DIP3_GPIO_Port GPIOE
#define DIP4_Pin GPIO_PIN_14
#define DIP4_GPIO_Port GPIOE
#define DIP5_Pin GPIO_PIN_15
#define DIP5_GPIO_Port GPIOE
#define DIP6_Pin GPIO_PIN_10
#define DIP6_GPIO_Port GPIOB
#define DIP7_Pin GPIO_PIN_11
#define DIP7_GPIO_Port GPIOB
#define WE_G_Pin GPIO_PIN_0
#define WE_G_GPIO_Port GPIOD
#define WE_Y_Pin GPIO_PIN_1
#define WE_Y_GPIO_Port GPIOD
#define WE_R_Pin GPIO_PIN_2
#define WE_R_GPIO_Port GPIOD
#define NS_G_Pin GPIO_PIN_3
#define NS_G_GPIO_Port GPIOD
#define NS_Y_Pin GPIO_PIN_4
#define NS_Y_GPIO_Port GPIOD
#define NS_R_Pin GPIO_PIN_5
#define NS_R_GPIO_Port GPIOD
#define STEPPER_A_Pin GPIO_PIN_9
#define STEPPER_A_GPIO_Port GPIOG
#define STEPPER_B_Pin GPIO_PIN_10
#define STEPPER_B_GPIO_Port GPIOG
#define STEPPER_C_Pin GPIO_PIN_11
#define STEPPER_C_GPIO_Port GPIOG
#define STEPPER_D_Pin GPIO_PIN_12
#define STEPPER_D_GPIO_Port GPIOG
#define TLC5615_CS_Pin GPIO_PIN_4
#define TLC5615_CS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern volatile uint16_t
  adc_value[2]; // 存放ADC值，adc_value[0]存放光敏电阻的采样值，adc_value[1]存放热敏电阻的采样值

typedef enum { LED_DISPLAY_LIGHT, LED_DISPLAY_TEMP } LED_DisplayMode_t; // 显示模式，显示光敏电阻还是热敏电阻
extern LED_DisplayMode_t LED_displayMode; // 当前显示模式

typedef enum {
    NS_GREEN_WE_RED,   // 状态0：南北绿，东西红
    NS_YELLOW_WE_RED,  // 状态1：南北黄，东西红
    NS_RED_WE_GREEN,   // 状态2：南北红，东西绿
    NS_RED_WE_YELLOW   // 状态3：南北红，东西黄
} TrafficLightState_t; // 交通灯状态
extern TrafficLightState_t trafficLightState; // 当前交通灯状态

typedef enum {
    RELAY_CONTROL,
    STEPPER_MOTOR_CONTROL,
    DC_MOTOR_CONTROL,
    SERVO_MOTOR_CONTROL
} DeviceControlMode_t; // 设备控制模式
extern volatile DeviceControlMode_t deviceControlMode; // 当前设备控制模式

typedef enum { MOTOR_OFF, MOTOR_ON } MotorStartStopState_t; // 电机开关状态
extern volatile MotorStartStopState_t motorStartStopState;  // 当前电机开关状态

typedef enum { MOTOR_MODE1, MOTOR_MODE2 } MotorMode_t; // 电机预设模式
extern volatile MotorMode_t motorMode;     // 当前电机预设模式

extern DMA_HandleTypeDef hdma_usart1_rx; // 声明DMA句柄，供main.c使用
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
