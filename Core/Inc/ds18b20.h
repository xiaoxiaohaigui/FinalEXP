/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file    ds18b20.h
	* @brief   DS18B20 driver interface.
	******************************************************************************
	*/
/* USER CODE END Header */

#ifndef __DS18B20_H__
#define __DS18B20_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

typedef struct
{
		int16_t temperature_x10;
} DS18B20_Data;

void DS18B20_Init(void); /* 初始化驱动（GPIO需配置为开漏输出并有上拉） */
int DS18B20_Read(DS18B20_Data *out); /* 读取温度，成功返回0 */
HAL_StatusTypeDef DS18B20_SendDma(const DS18B20_Data *data); /* DMA发送温度字符串 */
int DS18B20_ReadAndSendDma(void); /* 读取并DMA发送，成功返回0 */

#ifdef __cplusplus
}
#endif

#endif /* __DS18B20_H__ */
