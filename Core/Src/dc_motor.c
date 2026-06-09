#include "dc_motor.h"

#include "main.h"
#include "tim.h"

#define DCMOTOR_PWM_MAX_PERCENT 100U

static uint8_t dcMotorStarted = 0U;

/* ── 当前电机状态（用于占空比调整）──────────────────────────── */
static DCMotor_Direction_t currentDirection = DCMOTOR_BRAKE;
static uint8_t            currentDuty      = DCMOTOR_DEFAULT_DUTY_PERCENT;

/* ── 编码器测速状态（静态变量）────────────────────────────────── */
#define ENCODER_PULSES_PER_REV 100U /* 光电编码器线数（100 线）              */
#define ENCODER_COUNTS_PER_REV 400U /* TI12 模式 4 倍频：100×4 = 400 计数/转 */
#define ENCODER_TIM_PERIOD 65536U   /* 16 位计数器完整周期                   */

static uint8_t encoderStarted = 0U;   /* 编码器是否已启动                    */
static int32_t encoderPrevCnt = 0;    /* 上次调用时的计数器值（有符号语义）  */
static uint32_t encoderPrevTick = 0U; /* 上次调用时的系统毫秒时间戳          */
static int32_t motorSpeedDeci = 0;    /* 缓存的最近一次转速（deciRPM）       */

/* 确保 TIM4 的 CH1/CH2 PWM 只启动一次。 */
static void
DCMotor_StartPwm(void)
{
    if(dcMotorStarted == 0U)
    {
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
        dcMotorStarted = 1U;
    }
}

/* 将百分比占空比换算为 TIM4 比较值。 */
static void
DCMotor_SetCompare(uint32_t channel, uint8_t dutyPercent)
{
    uint8_t clampedDuty = dutyPercent;
    if(clampedDuty > DCMOTOR_PWM_MAX_PERCENT)
    {
        clampedDuty = DCMOTOR_PWM_MAX_PERCENT;
    }

    uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim4) + 1U;
    uint32_t compare = (period * clampedDuty) / DCMOTOR_PWM_MAX_PERCENT;
    __HAL_TIM_SET_COMPARE(&htim4, channel, compare);
}

/* 初始化直流电机驱动，默认进入制动状态。 */
void
DCMotor_Init(void)
{
    DCMotor_StartPwm();
    DCMotor_Brake();
}

/* 制动：CH1 和 CH2 都输出低电平。 */
void
DCMotor_Brake(void)
{
    DCMotor_StartPwm();
    DCMotor_SetCompare(TIM_CHANNEL_1, 0U);
    DCMotor_SetCompare(TIM_CHANNEL_2, 0U);
}

/* 统一设置直流电机方向和输出强度。 */
void
DCMotor_Set(DCMotor_Direction_t direction, uint8_t dutyPercent)
{
    DCMotor_StartPwm();

    /* 记录当前状态（制动/零占空比也记录） */
    currentDirection = direction;
    currentDuty      = dutyPercent;

    if(direction == DCMOTOR_BRAKE || dutyPercent == 0U)
    {
        DCMotor_Brake();
        return;
    }

    switch(direction)
    {
        case DCMOTOR_FORWARD:
            DCMotor_SetCompare(TIM_CHANNEL_2, 0U);
            DCMotor_SetCompare(TIM_CHANNEL_1, dutyPercent);
            break;

        case DCMOTOR_REVERSE:
            DCMotor_SetCompare(TIM_CHANNEL_1, 0U);
            DCMotor_SetCompare(TIM_CHANNEL_2, dutyPercent);
            break;

        default:
            DCMotor_Brake();
            break;
    }
}

/* 正转封装。 */
void
DCMotor_Forward(uint8_t dutyPercent)
{
    DCMotor_Set(DCMOTOR_FORWARD, dutyPercent);
}

/* 反转封装。 */
void
DCMotor_Reverse(uint8_t dutyPercent)
{
    DCMotor_Set(DCMOTOR_REVERSE, dutyPercent);
}

/* ── 编码器测速实现 ────────────────────────────────────────────── */

/**
 * @brief  启动 TIM3 编码器接口（仅执行一次）。
 */
static void
DCMotor_StartEncoder(void)
{
    if(encoderStarted == 0U)
    {
        HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
        encoderStarted = 1U;
    }
}

/**
 * @brief  将 TIM3 编码器计数器置零，并复位转速跟踪状态。
 */
void
DCMotor_ResetEncoder(void)
{
    DCMotor_StartEncoder();

    __HAL_TIM_SET_COUNTER(&htim3, 0);

    encoderPrevCnt = 0;
    encoderPrevTick = HAL_GetTick();
    motorSpeedDeci = 0;
}

/**
 * @brief  根据编码器计数值和系统时间计算当前转速（deciRPM）。
 * @retval 转速 × 10（deciRPM），正值对应编码器计数增加的方向，负值对应减少的方向。
 */
int32_t
DCMotor_GetSpeedDeciRPM(void)
{
    DCMotor_StartEncoder();

    /* ── 读取当前编码器计数值 ──────────────────────────── */
    /* 计数器为 16 位，用有符号语义解释以处理方向变化。       */
    int16_t currCnt16 = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
    uint32_t currTick = HAL_GetTick();

    /* ── 计算计数增量（处理 16 位翻转）─────────────────── */
    int32_t countDelta = (int32_t)currCnt16 - encoderPrevCnt;

    if(countDelta > 32767)
    {
        countDelta -= (int32_t)ENCODER_TIM_PERIOD; /* 正向翻转：+65535 → +1  */
    }
    else if(countDelta < -32768)
    {
        countDelta += (int32_t)ENCODER_TIM_PERIOD; /* 反向翻转：-65535 → -1  */
    }

    /* ── 计算时间增量（处理 HAL tick 32 位翻转）─────── */
    uint32_t tickDelta;
    if(currTick >= encoderPrevTick)
    {
        tickDelta = currTick - encoderPrevTick;
    }
    else
    {
        tickDelta = (0xFFFFFFFFU - encoderPrevTick) + currTick + 1U;
    }

    /* ── 更新历史值 ─────────────────────────────────── */
    encoderPrevCnt = (int32_t)currCnt16;
    encoderPrevTick = currTick;

    /* ── 计算 deciRPM = RPM × 10（纯整型）───────────── */
    /*  RPM × 10 = (countDelta × 60000 × 10) / (400 × ms)    */
    /*           = (countDelta × 600000) / (400 × ms)         */
    /*           = (countDelta × 1500) / ms                   */
    /*  用 int64_t 防中间溢出。                               */
    if(tickDelta > 0U)
    {
        int64_t num = (int64_t)countDelta * 1500LL;
        int64_t den = (int64_t)tickDelta;
        motorSpeedDeci = (int32_t)(num / den);
    }
    /* tickDelta == 0 时保留上一次转速，避免除零。 */

    return motorSpeedDeci;
}

/* ── 占空比调整接口 ────────────────────────────────────────────── */

/**
 * @brief  在当前方向基础上调整占空比，结果钳位在 0~100。
 * @param  delta  有符号增量（正 = 增加占空比，负 = 减少占空比）。
 * @note   若当前处于制动状态且 delta > 0，默认以正转方向启动。
 *         调整后通过 DCMotor_Set 重新应用，保持原方向。
 */
void
DCMotor_AdjustDuty(int8_t delta)
{
    /* 确定当前方向：若处于制动/零占空比且要求增加，默认正转 */
    DCMotor_Direction_t dir = currentDirection;
    if(dir == DCMOTOR_BRAKE && delta > 0)
    {
        dir = DCMOTOR_FORWARD;
    }

    /* 计算新占空比并钳位 */
    int16_t newDuty = (int16_t)currentDuty + (int16_t)delta;
    if(newDuty < 0)
    {
        newDuty = 0;
    }
    else if(newDuty > 100)
    {
        newDuty = 100;
    }

    DCMotor_Set(dir, (uint8_t)newDuty);
}

/**
 * @brief  获取当前占空比。
 * @retval 占空比，0~100。
 */
uint8_t
DCMotor_GetDuty(void)
{
    return currentDuty;
}
