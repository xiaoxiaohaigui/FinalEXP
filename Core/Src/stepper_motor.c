#include "stepper_motor.h"

#include "DelayUs.h"
#include "main.h"

#define STEPPER_STEPS_PER_REV 4096UL

#ifndef STEPPER_ACTIVE_LEVEL
#define STEPPER_ACTIVE_LEVEL GPIO_PIN_RESET
#endif

#define STEPPER_INACTIVE_LEVEL                                                                     \
    ((STEPPER_ACTIVE_LEVEL == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET)

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} StepperPhase_t;

static const StepperPhase_t stepperPhases[4] = {
    { STEPPER_A_GPIO_Port, STEPPER_A_Pin },
    { STEPPER_B_GPIO_Port, STEPPER_B_Pin },
    { STEPPER_C_GPIO_Port, STEPPER_C_Pin },
    { STEPPER_D_GPIO_Port, STEPPER_D_Pin },
};

// 四相八拍序列，8行对应8拍，4列对应4个相位，1表示导通，0表示关断。
static const uint8_t stepperSequence[8][4] = {
    { 1U, 0U, 0U, 0U }, { 1U, 1U, 0U, 0U }, { 0U, 1U, 0U, 0U }, { 0U, 1U, 1U, 0U },
    { 0U, 0U, 1U, 0U }, { 0U, 0U, 1U, 1U }, { 0U, 0U, 0U, 1U }, { 1U, 0U, 0U, 1U },
};

/* 将四相占空表写入到 GPIO 引脚，pattern 中 1 表示导通，0 表示关断。 */
static void
Stepper_WritePattern(const uint8_t pattern[4])
{
    for(uint8_t i = 0U; i < 4U; ++i)
    {
        HAL_GPIO_WritePin(stepperPhases[i].port,
                          stepperPhases[i].pin,
                          pattern[i] ? STEPPER_ACTIVE_LEVEL : STEPPER_INACTIVE_LEVEL);
    }
}

/* 按给定方向和步数执行八拍序列。direction >= 0 表示正序，< 0 表示反序。 */
static void
Stepper_Run(uint32_t stepCount, uint32_t stepDelayUs, int direction)
{
    if(stepCount == 0U)
    {
        return;
    }

    if(stepDelayUs == 0U)
    {
        stepDelayUs = 1U;
    }

    uint16_t delayUs = (stepDelayUs > UINT16_MAX) ? UINT16_MAX : (uint16_t)stepDelayUs;

    if(direction >= 0)
    {
        for(uint32_t stepIndex = 0U; stepIndex < stepCount; ++stepIndex)
        {
            Stepper_WritePattern(stepperSequence[stepIndex & 0x07U]);
            Delay_us(delayUs);
        }
    }
    else
    {
        for(uint32_t stepIndex = 0U; stepIndex < stepCount; ++stepIndex)
        {
            Stepper_WritePattern(stepperSequence[7U - (stepIndex & 0x07U)]);
            Delay_us(delayUs);
        }
    }
}

/* 初始化步进电机驱动状态。 */
void
Stepper_Init(void)
{
    Stepper_Stop();
}

/* 关闭全部相位输出。 */
void
Stepper_Stop(void)
{
    const uint8_t offPattern[4] = { 0U, 0U, 0U, 0U };
    Stepper_WritePattern(offPattern);
}

/* 正转指定步数。 */
void
Stepper_Forward(uint32_t stepCount, uint32_t stepDelayUs)
{
    Stepper_Run(stepCount, stepDelayUs, 1);
}

/* 反转指定步数。 */
void
Stepper_Reverse(uint32_t stepCount, uint32_t stepDelayUs)
{
    Stepper_Run(stepCount, stepDelayUs, -1);
}

/* 按角度旋转，按 1:64 减速四相八拍电机的 4096 步/圈进行换算，并做 8 倍角度校准。 */
void
Stepper_RotateAngle(float angleDeg, uint32_t stepDelayUs)
{
    if(angleDeg == 0.0f)
    {
        return;
    }

    uint32_t stepCount = (uint32_t)(((angleDeg > 0.0f ? angleDeg : -angleDeg) *
                                     STEPPER_ANGLE_SCALE * (float)STEPPER_STEPS_PER_REV / 360.0f) +
                                    0.5f);

    if(stepCount == 0U)
    {
        return;
    }

    if(angleDeg > 0.0f)
    {
        Stepper_Forward(stepCount, stepDelayUs);
    }
    else
    {
        Stepper_Reverse(stepCount, stepDelayUs);
    }
}
