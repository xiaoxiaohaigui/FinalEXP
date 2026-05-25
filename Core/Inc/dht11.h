/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file    dht11.h
	* @brief   DHT11 driver interface.
	******************************************************************************
	*/
/* USER CODE END Header */

#ifndef __DHT11_H__
#define __DHT11_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

typedef struct
{
		uint8_t humidity_int;
		uint8_t humidity_dec;
		uint8_t temperature_int;
		uint8_t temperature_dec;
} DHT11_Data;

void DHT11_Init(void); // 初始化驱动（依赖外部已配置GPIO）
int DHT11_Read(DHT11_Data *out); // 读取温湿度数据，成功返回0
HAL_StatusTypeDef DHT11_SendDma(const DHT11_Data *data); // 使用UART1 DMA发送测量值，成功返回HAL_OK
int DHT11_ReadAndSendDma(void); // 读取并通过DMA发送，成功返回0

#ifdef __cplusplus
}
#endif

#endif /* __DHT11_H__ */
