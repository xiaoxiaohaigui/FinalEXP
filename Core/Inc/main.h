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
#define LCD12864_CS_Pin GPIO_PIN_4
#define LCD12864_CS_GPIO_Port GPIOA
#define LCD12864_MOSI_Pin GPIO_PIN_5
#define LCD12864_MOSI_GPIO_Port GPIOA
#define LCD12864_CLK_Pin GPIO_PIN_6
#define LCD12864_CLK_GPIO_Port GPIOA
#define LCD12864_RST_Pin GPIO_PIN_7
#define LCD12864_RST_GPIO_Port GPIOA
#define LED11_Pin GPIO_PIN_11
#define LED11_GPIO_Port GPIOF

/* USER CODE BEGIN Private defines */
extern volatile uint16_t adc_value[2]; // 存放ADC值，adc_value[0]存放光敏电阻的采样值，adc_value[1]存放热敏电阻的采样值
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
