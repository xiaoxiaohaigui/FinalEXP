/**
 * @file    ch451.c
 * @brief   CH451 数码管显示驱动 + 矩阵键盘扫描 驱动程序
 *
 * @note    CH451 4线串行接口:
 *          - DIN  : 串行数据输入 (PG4)
 *          - DCLK : 串行时钟     (PG5)
 *          - LOAD : 数据加载     (PG3)
 *          - DOUT : 数据输出     (PG2, 输入)
 *
 *          协议: 12位命令, LSB first
 *          DCLK上升沿 CH451采样DIN, DCLK下降沿 CH451输出到DOUT
 *          LOAD上升沿 CH451锁存命令
 *
 *          GPIO 输出模式在 Init 中改为推挽输出(Push-Pull),
 *          以提供干净快速的信号边沿 (开漏依赖CH451弱上拉, 可能不够快)
 */

#include "ch451.h"
#include "main.h"
#include "DelayUs.h"
#include "cmsis_os.h"

/*===========================================================================
 * 低层GPIO操作宏
 *===========================================================================*/

#define CH451_DIN_HIGH()  HAL_GPIO_WritePin(CH451_DIN_GPIO_Port,  CH451_DIN_Pin,  GPIO_PIN_SET)
#define CH451_DIN_LOW()   HAL_GPIO_WritePin(CH451_DIN_GPIO_Port,  CH451_DIN_Pin,  GPIO_PIN_RESET)
#define CH451_DCLK_HIGH() HAL_GPIO_WritePin(CH451_DCLK_GPIO_Port, CH451_DCLK_Pin, GPIO_PIN_SET)
#define CH451_DCLK_LOW()  HAL_GPIO_WritePin(CH451_DCLK_GPIO_Port, CH451_DCLK_Pin, GPIO_PIN_RESET)
#define CH451_LOAD_HIGH() HAL_GPIO_WritePin(CH451_LOAD_GPIO_Port, CH451_LOAD_Pin, GPIO_PIN_SET)
#define CH451_LOAD_LOW()  HAL_GPIO_WritePin(CH451_LOAD_GPIO_Port, CH451_LOAD_Pin, GPIO_PIN_RESET)
#define CH451_DOUT_READ() HAL_GPIO_ReadPin(CH451_DOUT_GPIO_Port,  CH451_DOUT_Pin)

/*===========================================================================
 * 通信时序参数
 *
 * CH451 手册: DCLK 可 >10MHz (周期 <100ns)
 * 此处使用极保守的 ~100kHz (10us 周期) 以确保可靠通信
 *===========================================================================*/
#define CH451_DELAY_US   10   /* DCLK半周期 / LOAD脉冲宽度 */

/*===========================================================================
 * 内部函数
 *===========================================================================*/

static void CH451_SendBits(uint16_t data, uint8_t num_bits);
static uint8_t CH451_ReadBit(void);

/*===========================================================================
 * 内部函数实现
 *===========================================================================*/

/**
 * @brief  向 CH451 串行发送数据位 (LSB first)
 *
 *         每 bit 时序 (DCLK 初始为 HIGH):
 *           1. DIN = 当前位值
 *           2. DCLK ↓
 *           3. 延时
 *           4. DCLK ↑  →  CH451 在上升沿采样 DIN
 *           5. 延时
 */
static void
CH451_SendBits(uint16_t data, uint8_t num_bits)
{
    for(uint8_t i = 0; i < num_bits; i++)
    {
        if(data & (1 << i))
            CH451_DIN_HIGH();
        else
            CH451_DIN_LOW();

        /* DCLK 负脉冲: 下降沿 → 延时 → 上升沿(CH451采样) → 延时 */
        CH451_DCLK_LOW();
        Delay_us(CH451_DELAY_US);
        CH451_DCLK_HIGH();
        Delay_us(CH451_DELAY_US);
    }
}

/**
 * @brief  产生一个 DCLK 完整周期, 并在下降沿后读取 DOUT
 * @retval 读取到的位
 *
 *         时序 (DCLK 初始为 HIGH):
 *           1. DCLK ↓  →  CH451 在 DOUT 输出下一位
 *           2. 延时
 *           3. 读 DOUT
 *           4. DCLK ↑  →  CH451 可采样 DIN (此时 DIN 任意)
 *           5. 延时
 */
static uint8_t
CH451_ReadBit(void)
{
    uint8_t bit;

    CH451_DCLK_LOW();
    Delay_us(CH451_DELAY_US);
    bit = (CH451_DOUT_READ() == GPIO_PIN_SET) ? 1 : 0;

    CH451_DCLK_HIGH();
    Delay_us(CH451_DELAY_US);

    return bit;
}

/*===========================================================================
 * 公开 API 实现
 *===========================================================================*/

/**
 * @brief  CH451 初始化
 *
 *         流程:
 *           1. 总线初始电平
 *           2. DIN 低电平脉冲 → 启用 4 线串行接口
 *           3. 芯片内部复位
 *           4. 设定系统参数 → 启用显示+键盘
 *           5. 设定显示参数 → BCD 译码, 8 位扫描, 满亮度
 *
 *         GPIO 输出模式由 CubeMX 配置 (推挽输出)。
 */
void
CH451_Init(void)
{
    /*---------------------------------------------------------------
     * 步骤 1: 建立总线初始电平 (全部 HIGH)
     *   GPIO 输出模式由 CubeMX 配置 (推挽输出)
     *---------------------------------------------------------------*/
    CH451_DIN_HIGH();
    CH451_DCLK_HIGH();
    CH451_LOAD_HIGH();

    /* 等待 CH451 上电稳定 */
    osDelay(50);

    /*---------------------------------------------------------------
     * 步骤 2: DIN 低电平脉冲 → 启用 4 线串行接口
     *
     *   !!! 这是最关键的一步 !!!
     *   手册原文:
     *   "在 CH451 上电复位后，单片机必须在 DCLK 输出串行时钟之前，
     *    先在 DIN 上输出一个低电平脉冲（高→低→高）"
     *
     *   必须在任何 DCLK 跳变之前完成!
     *   使用较长脉冲宽度 (50ms) 确保 CH451 可靠检测
     *---------------------------------------------------------------*/
    CH451_DIN_LOW();
    osDelay(50);     /* 低电平保持 50ms */
    CH451_DIN_HIGH();
    osDelay(50);     /* 恢复后稳定 50ms */

    /*---------------------------------------------------------------
     * 步骤 3: 芯片内部复位
     *   命令: 001000000001B = 0x0201
     *---------------------------------------------------------------*/
    CH451_WriteCmd(CH451_CMD_RESET);
    osDelay(10);

    /*---------------------------------------------------------------
     * 步骤 4: 设定系统参数
     *   命令格式: 0100000[CKHF][DPLR][WDOG][KEYB][DISP]
     *   CKHF=0  DPLR=0(共阴)  WDOG=0  KEYB=1  DISP=1
     *   = 0100 0000 0011 = 0x0403
     *---------------------------------------------------------------*/
    CH451_WriteCmd(CH451_CMD_SYS_PARAM | CH451_SYS_KEYB | CH451_SYS_DISP);

    /*---------------------------------------------------------------
     * 步骤 5: 设定显示参数
     *   命令格式: 0101[MODE][LIMIT][INTENSITY]
     *   MODE=1(BCD)  LIMIT=000(8位扫描)  INTENSITY=0000(16/16占空比)
     *   = 0101 1000 0000 = 0x0580
     *---------------------------------------------------------------*/
    CH451_WriteCmd(CH451_CMD_DISP_PARAM | CH451_MODE_BCD | CH451_LIMIT_8 | CH451_INTENSITY_FULL);
}

/**
 * @brief  向 CH451 发送 12 位操作命令
 * @param  cmd : 12 位命令, B11..B0 (LSB first 发送)
 */
void
CH451_WriteCmd(uint16_t cmd)
{
    CH451_SendBits(cmd, 12);

    /* LOAD 上升沿: CH451 锁存移位寄存器并执行 */
    CH451_LOAD_LOW();
    Delay_us(CH451_DELAY_US);
    CH451_LOAD_HIGH();
    Delay_us(CH451_DELAY_US);
}

/**
 * @brief  读取按键代码
 * @retval 7 位按键代码
 *
 *         读取流程 (参考手册简化描述 1^1^1^0^↑H^H^L^L^L^H^H^):
 *           发命令 0111 → LOAD↑ → 读 DOUT(K6)
 *           → DCLK↓ → 读 DOUT(K5) → DCLK↑
 *           → ... 重复 6 次 → 得到 K5~K0
 */
uint8_t
CH451_ReadKeyCode(void)
{
    uint8_t keycode = 0;

    /* 发送读取按键命令 (0111 xxxx xxxx) */
    CH451_SendBits(CH451_CMD_READ_KEY, 12);

    /* LOAD↑ → CH451 输出 K6 到 DOUT */
    CH451_LOAD_LOW();
    Delay_us(CH451_DELAY_US);
    CH451_LOAD_HIGH();

    /* 读 K6 */
    if(CH451_DOUT_READ() == GPIO_PIN_SET)
        keycode |= 0x40;

    /* 读 K5~K0 (每 DCLK↓ 一位, 高位在前) */
    for(int8_t i = 5; i >= 0; i--)
    {
        if(CH451_ReadBit())
            keycode |= (1 << i);
    }

    return keycode;
}

/**
 * @brief  在指定数码管以 BCD 译码模式显示字符
 * @param  digit : DIG0~DIG7 (0~7)
 * @param  ch    : BCD 字符 0x00~0x1A
 * @param  dp    : 小数点 (0=灭, 非0=亮)
 *
 * @note   BCD 模式字数据: [DP(bit7)] [x(bit6)] [x(bit5)] [BCD(bit4-0)]
 */
void
CH451_DisplayBCD(uint8_t digit, uint8_t ch, uint8_t dp)
{
    uint8_t data;

    if(digit > 7) return;

    data = (ch & 0x1F) | (dp ? CH451_DP : 0);

    /* 命令: 1[DIG_ADDR(3)][DIG_DATA(8)] */
    CH451_WriteCmd(CH451_CMD_LOAD_DIG(digit) | data);
}

/**
 * @brief  以非译码模式显示段码
 * @param  digit    : DIG0~DIG7
 * @param  seg_data : bit7=DP, bit6=G, ..., bit0=A
 */
void
CH451_DisplayRaw(uint8_t digit, uint8_t seg_data)
{
    if(digit > 7) return;
    CH451_WriteCmd(CH451_CMD_LOAD_DIG(digit) | (seg_data & 0xFF));
}

/**
 * @brief  显示 8 位整数 (DIG7 最高位 ~ DIG0 最低位)
 */
void
CH451_DisplayNum(uint32_t num)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        CH451_DisplayBCD(i, num % 10, 0);
        num /= 10;
    }
}

/**
 * @brief  在指定范围显示整数
 * @param  start        : 起始位 0~7 (最低位)
 * @param  len          : 位数
 * @param  leading_zero : 0=前导零消隐, 1=显示前导零
 */
void
CH451_DisplayNumEx(uint32_t num, uint8_t start, uint8_t len, uint8_t leading_zero)
{
    uint8_t i;
    uint8_t digit;

    if(start > 7 || len == 0 || len > 8) return;
    if(start + len > 8) len = 8 - start;

    for(i = 0; i < len; i++)
    {
        digit = num % 10;
        num /= 10;

        if(digit == 0 && num == 0 && i < len - 1)
            CH451_DisplayBCD(start + i, leading_zero ? CH451_CHAR_0 : CH451_CHAR_SPACE, 0);
        else
            CH451_DisplayBCD(start + i, digit, 0);
    }
}

/**
 * @brief  清除所有显示
 */
void
CH451_ClearDisplay(void)
{
    for(uint8_t i = 0; i < 8; i++)
        CH451_DisplayBCD(i, CH451_CHAR_SPACE, 0);
}

/**
 * @brief  设置闪烁控制
 */
void
CH451_SetBlink(uint8_t blink_bits)
{
    CH451_WriteCmd(CH451_CMD_BLINK | (blink_bits & 0xFF));
}

/**
 * @brief  芯片内部复位
 */
void
CH451_Reset(void)
{
    CH451_WriteCmd(CH451_CMD_RESET);
}

/**
 * @brief  检测按键中断 (DOUT == LOW?)
 */
/**
 * @brief  在数码管上以矩阵方式显示按键位置
 * @param  row     : 按键行号 SEG0~SEG7 (0~7)
 * @param  col     : 按键列号 DIG0~DIG7 (0~7)
 * @param  pressed : 1=按下(显示行号), 0=释放(清除)
 *
 * @note   8 位数码管 DIG0~DIG7 分别对应键盘的 8 列,
 *         每位数码管显示该列当前按下的键的行号 (0~7)。
 *         例如: SEG4 行 DIG3 列的键按下 → DIG3 显示 "4"
 */
void
CH451_DisplayKeyMatrix(uint8_t row, uint8_t col, uint8_t pressed)
{
    if(row > 7 || col > 7) return;

    if(pressed)
        CH451_DisplayBCD(col, row, 0);
    else
        CH451_DisplayBCD(col, CH451_CHAR_SPACE, 0);
}

uint8_t
CH451_KeyPressed(void)
{
    return (CH451_DOUT_READ() == GPIO_PIN_RESET) ? 1 : 0;
}
