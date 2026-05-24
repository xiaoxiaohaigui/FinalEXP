#include "dc_motor.h"

#include "main.h"
#include "tim.h"

#define DCMOTOR_PWM_MAX_PERCENT 100U

static uint8_t dcMotorStarted = 0U;

/* 确保 TIM1 的 CH1/CH2 PWM 只启动一次。 */
static void
DCMotor_StartPwm(void)
{
    if(dcMotorStarted == 0U)
    {
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
        dcMotorStarted = 1U;
    }
}

/* 将百分比占空比换算为 TIM1 比较值。 */
static void
DCMotor_SetCompare(uint32_t channel, uint8_t dutyPercent)
{
    uint8_t clampedDuty = dutyPercent;
    if(clampedDuty > DCMOTOR_PWM_MAX_PERCENT)
    {
        clampedDuty = DCMOTOR_PWM_MAX_PERCENT;
    }

    uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim1) + 1U;
    uint32_t compare = (period * clampedDuty) / DCMOTOR_PWM_MAX_PERCENT;
    __HAL_TIM_SET_COMPARE(&htim1, channel, compare);
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
