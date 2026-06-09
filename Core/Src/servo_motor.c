#include "servo_motor.h"
#include "cmsis_os.h"
#include "tim.h"

void
ServoMotor_Init(void)
{
    // TIM8 CH4 配置为 PWM 输出，频率 50Hz，占空比初始值为 2.5%（对应舵机的 0 度位置）
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 50);
}

// 舵机归零
void
ServoMotor_Home(void)
{
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 50); // 接近 0 度
}

// 预设模式 1：舵机在 0 度和 180 度之间来回摆动
void
ServoMotor_Mode1(void)
{
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 250); // 接近 180 度
    osDelay(1200);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 50); // 接近 0 度
    osDelay(1200);
}

// 预设模式 2：舵机在 0 度和 90 度之间来回摆动
void
ServoMotor_Mode2(void)
{
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 150); // 接近 90 度
    osDelay(1200);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 50); // 接近 0 度
    osDelay(1200);
}