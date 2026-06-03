/**
 ******************************************************************************
 * @file           : ir_receiver.c
 * @brief          : NEC 红外接收驱动 — 实现
 * @description    : 使用 TIM3（CubeMX 配置的 1MHz/1µs 时基）和 PG0 上的
 *                   EXTI0 双沿捕获来解码 NEC 格式的红外遥控信号。
 *                   解码后的数据通过 USART1 DMA 发送到上位机。
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "ir_receiver.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

/* 外部句柄 ------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/* ---------------------------------------------------------------------------*/
/* 红外解码状态机                                                              */
/* ---------------------------------------------------------------------------*/
typedef enum
{
    IR_STATE_IDLE,            /* 等待第一个下降沿（引导码开始）                 */
    IR_STATE_LEADER_LOW,      /* 正在接收 9ms 引导码低电平                      */
    IR_STATE_LEADER_HIGH,     /* 正在接收引导码高电平（4.5ms=正常 / 2.25ms=重复） */
    IR_STATE_DATA,            /* 正在接收 32 位数据                             */
    IR_STATE_DONE,            /* 一帧解码完成，等待主循环读取                   */
} IR_State_t;

/* 解码结果 ------------------------------------------------------------------*/
static volatile IR_State_t ir_state       = IR_STATE_IDLE;
static volatile uint32_t   ir_frame       = 0;        /* 32 位 NEC 帧         */
static volatile uint8_t    ir_bit_count   = 0;        /* 已接收的位数          */
static volatile uint8_t    ir_data_ready  = 0;        /* 数据就绪标志          */
static volatile uint8_t    ir_is_repeat   = 0;        /* 是否重复码            */
static          uint8_t    ir_last_command = 0;       /* 上一次的命令码（重复码时使用） */

/* 上一次边沿的定时器计数值 --------------------------------------------------*/
static volatile uint32_t   ir_last_edge_time = 0;

/* ---------------------------------------------------------------------------*/
/* 内部辅助函数                                                                */
/* ---------------------------------------------------------------------------*/

/**
 * @brief  安全读取 TIM3 自由运行计数器的当前值。
 * @note   TIM3 由 CubeMX 配置：72MHz / 72 = 1MHz，即 1 tick = 1µs。
 */
static inline uint32_t IR_Read_Timer(void)
{
    return __HAL_TIM_GET_COUNTER(&htim3);
}

/**
 * @brief  计算两次计数器采样之间的时间（µs），正确处理 16 位溢出。
 * @param  now:  当前计数器值
 * @param  prev: 上一次的计数器值
 * @retval 时间差，单位 µs
 */
static uint32_t IR_Duration(uint32_t now, uint32_t prev)
{
    if (now >= prev)
    {
        return now - prev;
    }
    else
    {
        /* 计数器在 65535 处回绕 */
        return (0xFFFFU - prev) + now + 1U;
    }
}

/**
 * @brief  判断测量值是否落在 [min, max] 区间内。
 */
static inline uint8_t IR_Time_In_Range(uint32_t val, uint32_t min, uint32_t max)
{
    return (val >= min) && (val <= max);
}

/**
 * @brief  复位状态机，丢弃当前帧。
 */
static void IR_Reset_State(void)
{
    ir_state          = IR_STATE_IDLE;
    ir_frame          = 0;
    ir_bit_count      = 0;
    ir_is_repeat      = 0;
    ir_last_edge_time = 0;
}

/* ---------------------------------------------------------------------------*/
/* 遥控器按键对照表                                                            */
/* ---------------------------------------------------------------------------*/
typedef struct
{
    uint8_t     command;        /* 命令码                                 */
    const char *name;           /* 按键名称                               */
} IR_Key_Entry_t;

static const IR_Key_Entry_t ir_key_table[] =
{
    {0x43, "PLAY/PAUSE"},
    {0x44, "PREV"},
    {0x40, "NEXT"},
    {0x45, "CH-"},
    {0x46, "CH"},
    {0x47, "CH+"},
    {0x07, "VOL-"},
    {0x15, "VOL+"},
    {0x09, "EQ"},
    {0x16, "0"},
    {0x0C, "1"},
    {0x18, "2"},
    {0x5E, "3"},
    {0x08, "4"},
    {0x1C, "5"},
    {0x5A, "6"},
    {0x42, "7"},
    {0x52, "8"},
    {0x4A, "9"},
    {0x19, "100+"},
    {0x0D, "200+"},
};
#define IR_KEY_COUNT  (sizeof(ir_key_table) / sizeof(ir_key_table[0]))

/**
 * @brief  根据命令码查找对应的按键名称。
 * @param  command: NEC 帧中的命令字节
 * @retval 按键名称字符串；未识别则返回 "UNKNOWN"。
 * @note   可在 IR_Read_Data() 和 IR_Send_Data_Over_UART() 之间调用，
 *         用于自定义按键处理逻辑。
 */
const char *IR_Get_Key_Name(uint8_t command)
{
    for (uint16_t i = 0; i < IR_KEY_COUNT; i++)
    {
        if (ir_key_table[i].command == command)
        {
            return ir_key_table[i].name;
        }
    }
    return "UNKNOWN";
}

/* ---------------------------------------------------------------------------*/
/* 公开 API                                                                   */
/* ---------------------------------------------------------------------------*/

/**
 * @brief  初始化红外接收器。
 * @note   INFRARED 引脚（PG0）和 EXTI0 中断已由 CubeMX 在 gpio.c 中配置。
 *         TIM3 已由 CubeMX 在 tim.c 中配置。
 *         调用本函数前必须先调用 Delay_Init() 启动 TIM3。
 */
void IR_Receiver_Init(void)
{
    /* 以干净的状态开始 */
    IR_Reset_State();
}

/**
 * @brief  边沿捕获回调 —— 在 INFRARED 引脚的每个上升沿和下降沿被调用。
 *         实现 NEC 协议状态机解码。
 *
 * @note   本函数在 ISR 上下文中运行（EXTI0_IRQHandler），务必保持快速。
 *         不要在 ISR 中使用 printf、HAL_Delay 或 UART 发送。
 */
void IR_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin != INFRARED_Pin)
    {
        return;
    }

    uint32_t now      = IR_Read_Timer();
    uint32_t duration = IR_Duration(now, ir_last_edge_time);
    ir_last_edge_time = now;

    /* 读取当前引脚电平 */
    uint8_t pin_is_high = (HAL_GPIO_ReadPin(INFRARED_GPIO_Port, INFRARED_Pin) == GPIO_PIN_SET);

    /* ── 状态机 ────────────────────────────────────────────── */
    switch (ir_state)
    {
    /* ------------------------------------------------------- */
    case IR_STATE_IDLE:
        /* 等待第一个下降沿（引导码开始）。                     */
        if (!pin_is_high)
        {
            ir_state = IR_STATE_LEADER_LOW;
            /* 此时 duration 为空闲间隔，不用于解码。           */
        }
        break;

    /* ------------------------------------------------------- */
    case IR_STATE_LEADER_LOW:
        /* 刚检测到上升沿 → ~9ms 引导码低电平结束。            */
        if (pin_is_high)
        {
            if (IR_Time_In_Range(duration, NEC_LEADER_LOW_MIN, NEC_LEADER_LOW_MAX))
            {
                ir_state = IR_STATE_LEADER_HIGH;
            }
            else
            {
                IR_Reset_State();   /* 噪声或未知协议           */
            }
        }
        else
        {
            IR_Reset_State();       /* 意外的下降沿             */
        }
        break;

    /* ------------------------------------------------------- */
    case IR_STATE_LEADER_HIGH:
        /* 刚检测到下降沿 → 引导码高电平阶段结束。             */
        if (!pin_is_high)
        {
            if (IR_Time_In_Range(duration, NEC_LEADER_HIGH_MIN, NEC_LEADER_HIGH_MAX))
            {
                /* 普通帧 → 开始接收数据                       */
                ir_state     = IR_STATE_DATA;
                ir_frame     = 0;
                ir_bit_count = 0;
                ir_is_repeat = 0;
            }
            else if (IR_Time_In_Range(duration, NEC_REPEAT_HIGH_MIN, NEC_REPEAT_HIGH_MAX))
            {
                /* 重复码 → 立即标记就绪                        */
                ir_is_repeat  = 1;
                ir_data_ready = 1;
                /* 复位状态但保留标志位                         */
                ir_state       = IR_STATE_IDLE;
                ir_frame       = 0;
                ir_bit_count   = 0;
            }
            else
            {
                IR_Reset_State();   /* 未知时序                 */
            }
        }
        else
        {
            IR_Reset_State();       /* 意外的上升沿             */
        }
        break;

    /* ------------------------------------------------------- */
    case IR_STATE_DATA:
        if (pin_is_high)
        {
            /* 上升沿 → 数据位低电平结束。                     */
            /* 校验低电平宽度 ~560µs（抗干扰检查）。           */
            if (!IR_Time_In_Range(duration, NEC_BIT_LOW_MIN, NEC_BIT_LOW_MAX))
            {
                IR_Reset_State();
            }
            /* 继续等待下降沿来解码比特值                       */
        }
        else
        {
            /* 下降沿 → 数据位高电平结束 → 解码比特值。        */
            if (ir_bit_count < 32U)
            {
                if (IR_Time_In_Range(duration, NEC_BIT_0_HIGH_MIN, NEC_BIT_0_HIGH_MAX))
                {
                    /* 逻辑 0：对应位保持 0（默认值）           */
                    ir_frame &= ~(1UL << ir_bit_count);
                    ir_bit_count++;
                }
                else if (IR_Time_In_Range(duration, NEC_BIT_1_HIGH_MIN, NEC_BIT_1_HIGH_MAX))
                {
                    /* 逻辑 1：对应位置 1                        */
                    ir_frame |= (1UL << ir_bit_count);
                    ir_bit_count++;
                }
                else
                {
                    /* 比特时序超出容差 → 丢弃本帧              */
                    IR_Reset_State();
                }
            }

            /* 32 位全部收齐？ */
            if (ir_bit_count == 32U)
            {
                /* 保存本次命令码，供后续重复码时查找按键名称 */
                ir_last_command = (uint8_t)((ir_frame >> 16) & 0xFFU);
                ir_data_ready = 1;
                ir_state      = IR_STATE_DONE;
            }
        }
        break;

    /* ------------------------------------------------------- */
    case IR_STATE_DONE:
        /* 帧已解码完成 —— 忽略后续边沿，直到主循环读取并清除。 */
        break;

    default:
        IR_Reset_State();
        break;
    }
}

/* ---------------------------------------------------------------------------*/
/* 数据访问接口                                                                */
/* ---------------------------------------------------------------------------*/

uint8_t IR_Is_Data_Ready(void)
{
    return ir_data_ready;
}

void IR_Clear_Data_Ready(void)
{
    ir_data_ready = 0;
    ir_is_repeat  = 0;
    IR_Reset_State();
}

uint32_t IR_Get_Frame(void)
{
    return ir_frame;
}

uint8_t IR_Get_Address(void)
{
    return (uint8_t)(ir_frame & 0xFFU);
}

uint8_t IR_Get_Command(void)
{
    return (uint8_t)((ir_frame >> 16) & 0xFFU);
}

/**
 * @brief  直接获取当前收到的按键名称（自动处理普通帧和重复码）。
 * @retval 按键名称字符串，未识别则返回 "UNKNOWN"。
 * @note   需在 IR_Is_Data_Ready() 返回 1 后调用。
 *         普通帧：返回本次按键名。
 *         重复码：返回上次按键名（长按遥控器时）。
 */
const char *IR_Get_Key(void)
{
    if (ir_is_repeat)
    {
        return IR_Get_Key_Name(ir_last_command);
    }
    else
    {
        uint8_t cmd = (uint8_t)((ir_frame >> 16) & 0xFFU);
        return IR_Get_Key_Name(cmd);
    }
}

uint8_t IR_Is_Repeat(void)
{
    return ir_is_repeat;
}

/**
 * @brief  【步骤1 — 读取】将解码后的红外数据格式化为可读字符串。
 * @param  buf:  输出缓冲区
 * @param  size: 缓冲区大小（字节）
 * @retval 格式化后字符串的长度（不含 '\0'）。
 * @note   调用者可在发送前修改 buf 的内容（例如追加按键名称）。
 */
uint16_t IR_Read_Data(char *buf, uint16_t size)
{
    if (buf == NULL || size == 0)
    {
        return 0;
    }

    if (ir_is_repeat)
    {
        /* 重复码：使用上一次保存的命令码查找按键名称              */
        return (uint16_t)snprintf(buf, size,
                    "[IR] REPEAT  Key=%s\r\n",
                    IR_Get_Key_Name(ir_last_command));
    }
    else
    {
        uint8_t addr     = (uint8_t)(ir_frame & 0xFFU);
        uint8_t addr_inv = (uint8_t)((ir_frame >> 8) & 0xFFU);
        uint8_t cmd      = (uint8_t)((ir_frame >> 16) & 0xFFU);
        uint8_t cmd_inv  = (uint8_t)((ir_frame >> 24) & 0xFFU);

        /* 反码校验：低字节应与高字节按位取反相等                  */
        uint8_t addr_ok  = (addr == (uint8_t)(~addr_inv));
        uint8_t cmd_ok   = (cmd  == (uint8_t)(~cmd_inv));

        return (uint16_t)snprintf(buf, size,
                    "[IR] Addr=0x%02X%s  Cmd=0x%02X%s  Key=%s  Raw=0x%08lX\r\n",
                    addr, addr_ok ? " OK" : " ERR",
                    cmd,  cmd_ok  ? " OK" : " ERR",
                    IR_Get_Key_Name(cmd),
                    (unsigned long)ir_frame);
    }
}

/**
 * @brief  【步骤2 — 发送】通过 USART1 DMA 将数据发送到上位机。
 * @param  data: 待发送的数据指针
 * @param  len:  数据长度（字节）
 * @note   使用 DMA 发送，函数立即返回，不等发送完成。
 */
void IR_Send_Data_Over_UART(const uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0)
    {
        return;
    }
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)data, len);
}

/**
 * @brief  覆盖 HAL 库的弱符号 GPIO EXTI 回调。
 *         将 EXTI0 事件路由到红外解码状态机。
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    IR_EXTI_Callback(GPIO_Pin);
}
