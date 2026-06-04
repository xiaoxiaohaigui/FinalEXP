#ifndef __CH451_H__
#define __CH451_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*===========================================================================
 * CH451 操作命令定义 (12位, 低位在前发送)
 *===========================================================================*/

/* 设定系统参数: 0100000[CKHF][DPLR][WDOG][KEYB][DISP] */
#define CH451_CMD_SYS_PARAM    0x0400 /* B11-B5: 0100000 */
#define CH451_SYS_CKHF         (1 << 4) /* B4: 高频时钟(快速闪烁) */
#define CH451_SYS_DPLR         (1 << 3) /* B3: DIG极性: 1=高有效(共阳), 0=低有效(共阴) */
#define CH451_SYS_WDOG         (1 << 2) /* B2: 看门狗使能 */
#define CH451_SYS_KEYB         (1 << 1) /* B1: 键盘扫描使能 */
#define CH451_SYS_DISP         (1 << 0) /* B0: 显示驱动使能 */

/* 设定显示参数: 0101[MODE][LIMIT(3)][INTENSITY(4)] */
#define CH451_CMD_DISP_PARAM   0x0500 /* B11-B8: 0101 */
#define CH451_MODE_BCD         (1 << 7) /* 1=BCD译码, 0=不译码 */
#define CH451_MODE_RAW         0
/* 扫描极限 LIMIT: 3位, B6-B4, 001B=1位 ~ 000B=8位 */
#define CH451_LIMIT(n)         (((n) & 0x07) << 4)
#define CH451_LIMIT_8          0x00 /* 000B = 8位数码管 */
#define CH451_LIMIT_7          0x10 /* 001B = 7位 */
#define CH451_LIMIT_6          0x20
#define CH451_LIMIT_5          0x30
#define CH451_LIMIT_4          0x40
#define CH451_LIMIT_3          0x50
#define CH451_LIMIT_2          0x60
#define CH451_LIMIT_1          0x70
/* 显示亮度 INTENSITY: 4位, B3-B0, 0000B=16/16, 0001B=1/16 ... 1111B=15/16 */
#define CH451_INTENSITY(n)     ((n) & 0x0F)
#define CH451_INTENSITY_FULL   0x00
#define CH451_INTENSITY_1_16   0x01
#define CH451_INTENSITY_8_16   0x08
#define CH451_INTENSITY_15_16  0x0F

/* 加载字数据: 1[DIG_ADDR(3)][DIG_DATA(8)] */
#define CH451_CMD_LOAD_DIG(n)  (0x0800 | (((n) & 0x07) << 8))
#define CH451_DIG0             0
#define CH451_DIG1             1
#define CH451_DIG2             2
#define CH451_DIG3             3
#define CH451_DIG4             4
#define CH451_DIG5             5
#define CH451_DIG6             6
#define CH451_DIG7             7

/* 读取按键代码: 0111xxxxxxxx */
#define CH451_CMD_READ_KEY     0x0700

/* 芯片内部复位: 001000000001 */
#define CH451_CMD_RESET         0x0201

/* 设定闪烁控制: 0110[D7S..D0S] */
#define CH451_CMD_BLINK         0x0600

/* 空操作 */
#define CH451_CMD_NOP           0x0000

/*===========================================================================
 * BCD 译码字符定义 (写入字数据的低5位)
 *===========================================================================*/
#define CH451_CHAR_0            0x00
#define CH451_CHAR_1            0x01
#define CH451_CHAR_2            0x02
#define CH451_CHAR_3            0x03
#define CH451_CHAR_4            0x04
#define CH451_CHAR_5            0x05
#define CH451_CHAR_6            0x06
#define CH451_CHAR_7            0x07
#define CH451_CHAR_8            0x08
#define CH451_CHAR_9            0x09
#define CH451_CHAR_A            0x0A
#define CH451_CHAR_b            0x0B
#define CH451_CHAR_c            0x0C
#define CH451_CHAR_d            0x0D
#define CH451_CHAR_E            0x0E
#define CH451_CHAR_F            0x0F
#define CH451_CHAR_SPACE        0x10
#define CH451_CHAR_PLUSMINUS    0x11
#define CH451_CHAR_MINUS        0x12
#define CH451_CHAR_EQUAL        0x13
#define CH451_CHAR_LBRACKET     0x14
#define CH451_CHAR_RBRACKET     0x15
#define CH451_CHAR_UNDERSCORE   0x16
#define CH451_CHAR_H            0x17
#define CH451_CHAR_L            0x18
#define CH451_CHAR_P            0x19
#define CH451_CHAR_DOT          0x1A

/* 小数点标志 */
#define CH451_DP                0x80

/*===========================================================================
 * 非BCD模式段码定义 (字数据位7-0 对应 DP G F E D C B A)
 *===========================================================================*/
#define CH451_SEG_A             0x01
#define CH451_SEG_B             0x02
#define CH451_SEG_C             0x04
#define CH451_SEG_D             0x08
#define CH451_SEG_E             0x10
#define CH451_SEG_F             0x20
#define CH451_SEG_G             0x40
#define CH451_SEG_DP            0x80

/* 按键代码特殊值 */
#define CH451_NO_KEY            0xFF /* 没有按键 */

/*===========================================================================
 * 按键代码结构
 *===========================================================================*/
/* 按键代码: [K6=状态][K5:K3=行(SEG)][K2:K0=列(DIG)]
 *          K6: 1=按下, 0=释放
 *          K5-K3: SEG7(111) ~ SEG0(000)
 *          K2-K0: DIG7(111) ~ DIG0(000)
 */
#define CH451_KEY_PRESSED       0x40 /* K6=1 表示按下 */
#define CH451_KEY_ROW_MASK      0x38 /* K5-K3 */
#define CH451_KEY_ROW_SHIFT     3
#define CH451_KEY_COL_MASK      0x07 /* K2-K0 */
#define CH451_KEY_COL_SHIFT     0

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  CH451 初始化
 * @note   使能4线串行接口，开启显示驱动和键盘扫描
 *         配置为BCD译码模式、8位扫描、16/16亮度
 */
void CH451_Init(void);

/**
 * @brief  向CH451发送12位操作命令
 * @param  cmd : 12位命令字
 */
void CH451_WriteCmd(uint16_t cmd);

/**
 * @brief  读取按键代码
 * @retval 7位按键代码 (最高位=状态, 0xFF表示无按键)
 * @note   仅在DOUT为低电平(有按键中断)时调用才能读到有效按键
 */
uint8_t CH451_ReadKeyCode(void);

/**
 * @brief  在指定数码管位显示字符(BCD模式)
 * @param  digit : 数码管位 0~7 (DIG0~DIG7)
 * @param  ch    : BCD字符值 (0x00~0x1A), 可使用CH451_CHAR_*宏
 * @param  dp    : 是否点亮小数点 (0=灭, 非0=亮)
 * @note   使用BCD译码模式
 */
void CH451_DisplayBCD(uint8_t digit, uint8_t ch, uint8_t dp);

/**
 * @brief  在指定数码管位以非译码模式显示段码
 * @param  digit : 数码管位 0~7 (DIG0~DIG7)
 * @param  seg_data : 段码数据 (bit7=DP, bit6=G, ... bit0=A)
 * @note   使用不译码模式时调用
 */
void CH451_DisplayRaw(uint8_t digit, uint8_t seg_data);

/**
 * @brief  显示8位无符号整数 (BCD模式, 无前导零消除)
 * @param  num : 要显示的数字 (0~99999999)
 * @note   从DIG7(最高位)到DIG0(最低位)显示
 */
void CH451_DisplayNum(uint32_t num);

/**
 * @brief  显示整数到指定的数码管范围
 * @param  num    : 要显示的数字
 * @param  start  : 起始数码管位 (0~7, 从低到高)
 * @param  len    : 显示位数 (1~8)
 * @param  leading_zero : 是否显示前导零 (0=不显示, 1=显示)
 */
void CH451_DisplayNumEx(uint32_t num, uint8_t start, uint8_t len, uint8_t leading_zero);

/**
 * @brief  清除所有数码管显示 (全部显示空格)
 */
void CH451_ClearDisplay(void);

/**
 * @brief  设置闪烁控制
 * @param  blink_bits : 8位, bit0=DIG0闪烁... bit7=DIG7闪烁, 1=闪烁
 */
void CH451_SetBlink(uint8_t blink_bits);

/**
 * @brief  芯片内部复位
 */
void CH451_Reset(void);

/**
 * @brief  检查是否有按键被按下
 * @retval 1=有按键, 0=无按键
 * @note   通过读取DOUT引脚电平判断
 */
void CH451_DisplayKeyMatrix(uint8_t row, uint8_t col, uint8_t pressed);

uint8_t CH451_KeyPressed(void);

/**
 * @brief  从按键代码中提取行号 (SEG0~SEG7 → 0~7)
 * @param  keycode : 7位按键代码
 * @retval 行号 0~7
 */
#define CH451_GetKeyRow(kc)  (((kc) & CH451_KEY_ROW_MASK) >> CH451_KEY_ROW_SHIFT)

/**
 * @brief  从按键代码中提取列号 (DIG0~DIG7 → 0~7)
 * @param  keycode : 7位按键代码
 * @retval 列号 0~7
 */
#define CH451_GetKeyCol(kc)  ((kc) & CH451_KEY_COL_MASK)

/**
 * @brief  判断按键是否是按下状态
 * @param  keycode : 7位按键代码
 * @retval 1=按下, 0=释放
 */
#define CH451_IsKeyPress(kc) (((kc) & CH451_KEY_PRESSED) != 0)

#ifdef __cplusplus
}
#endif

#endif /* __CH451_H__ */
