/**
 ******************************************************************************
 * @file           : ir_receiver.h
 * @brief          : NEC 红外接收驱动 — 头文件
 ******************************************************************************
 */

#ifndef __IR_RECEIVER_H__
#define __IR_RECEIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* NEC 协议时序常量（单位：微秒）----------------------------------------------*/
#define NEC_LEADER_LOW_MIN      8000U   /* 引导码低电平最小值（9ms）           */
#define NEC_LEADER_LOW_MAX      10000U  /* 引导码低电平最大值                   */
#define NEC_LEADER_HIGH_MIN     4000U   /* 引导码高电平最小值（4.5ms）          */
#define NEC_LEADER_HIGH_MAX     5000U   /* 引导码高电平最大值                   */
#define NEC_REPEAT_HIGH_MIN     1800U   /* 重复码高电平最小值（2.25ms）         */
#define NEC_REPEAT_HIGH_MAX     2700U   /* 重复码高电平最大值                   */
#define NEC_BIT_LOW_MIN          400U   /* 数据位低电平最小值（560µs）          */
#define NEC_BIT_LOW_MAX          750U   /* 数据位低电平最大值                   */
#define NEC_BIT_0_HIGH_MIN       350U   /* 逻辑0高电平最小值（560µs）           */
#define NEC_BIT_0_HIGH_MAX       800U   /* 逻辑0高电平最大值                    */
#define NEC_BIT_1_HIGH_MIN      1400U   /* 逻辑1高电平最小值（1690µs）          */
#define NEC_BIT_1_HIGH_MAX      2000U   /* 逻辑1高电平最大值                    */

/* NEC 帧格式: 地址(8) + 地址反码(8) + 命令(8) + 命令反码(8) = 32位          */

/* 公开 API ------------------------------------------------------------------*/

/**
 * @brief  初始化红外接收器（复位内部状态机）。
 * @note   TIM3 时基和 PG0/EXTI0 已由 CubeMX 在 tim.c / gpio.c 中配置完成。
 *         调用本函数前必须先调用 Delay_Init() 启动 TIM3 计数。
 */
void IR_Receiver_Init(void);

/**
 * @brief  EXTI 回调 —— 由 HAL_GPIO_EXTI_Callback 调用，
 *         将边沿事件传入红外解码状态机。
 * @param  GPIO_Pin: 触发中断的引脚（应为 INFRARED_Pin）
 */
void IR_EXTI_Callback(uint16_t GPIO_Pin);

/**
 * @brief  查询是否有一帧解码完成的数据待读取。
 * @retval 1 = 有数据待读取，0 = 无数据
 */
uint8_t IR_Is_Data_Ready(void);

/**
 * @brief  清除"数据就绪"标志，复位状态机以接收下一帧。
 */
void IR_Clear_Data_Ready(void);

/**
 * @brief  获取解码后的原始 32 位 NEC 帧。
 * @retval 32 位帧: [地址(8) | 地址反码(8) | 命令(8) | 命令反码(8)]
 */
uint32_t IR_Get_Frame(void);

/**
 * @brief  获取帧中的地址字节。
 */
uint8_t IR_Get_Address(void);

/**
 * @brief  获取帧中的命令字节。
 */
uint8_t IR_Get_Command(void);

/**
 * @brief  查询最近收到的编码是否为重复码。
 * @retval 1 = 重复码，0 = 正常帧
 */
uint8_t IR_Is_Repeat(void);

/**
 * @brief  直接获取当前收到的按键名称（自动处理普通帧和重复码）。
 * @retval 按键名称字符串；未识别则返回 "UNKNOWN"。
 * @note   需在 IR_Is_Data_Ready() 返回 1 后调用，一步到位获取按键名。
 */
const char *IR_Get_Key(void);

/**
 * @brief  根据命令码查找对应的按键名称。
 * @param  command: NEC 帧中的命令字节
 * @retval 按键名称字符串（如 "VOL+"、"PLAY/PAUSE"）；未识别则返回 "UNKNOWN"。
 * @note   可在 IR_Read_Data() 和 IR_Send_Data_Over_UART() 之间调用，
 *         用于自定义按键处理逻辑。
 */
const char *IR_Get_Key_Name(uint8_t command);

/**
 * @brief  【步骤1 — 读取】将解码后的红外数据格式化为可读字符串。
 * @param  buf:  输出缓冲区
 * @param  size: 缓冲区大小（字节）
 * @retval 格式化后字符串的长度（不含 '\0'），0 表示失败。
 * @note   输出格式（自动包含按键名称）：
 *         普通帧: "[IR] Addr=0x00 OK  Cmd=0x45 OK  Key=CH-  Raw=0xFF00BA45\r\n"
 *         重复码: "[IR] REPEAT  Key=CH-\r\n"
 *         地址/命令后会标 "OK"（反码校验通过）或 "ERR"（校验失败）。
 */
uint16_t IR_Read_Data(char *buf, uint16_t size);

/**
 * @brief  【步骤2 — 发送】通过 USART1 DMA 将数据发送到上位机。
 * @param  data: 待发送的数据指针
 * @param  len:  数据长度（字节）
 * @note   此函数为通用发送函数，可发送任意数据。
 *         发送使用 DMA，函数立即返回，不等待发送完成。
 */
void IR_Send_Data_Over_UART(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __IR_RECEIVER_H__ */
