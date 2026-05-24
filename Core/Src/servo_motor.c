#include "servo_motor.h"
#include "cmsis_os.h"
#include "tim.h"

void
ServoMotor_Init(void)
{
    // TIM8 CH4配置为PWM输出，频率50Hz，初始占空比0%（对应1ms脉宽）
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 0);
}

// 舵机归零
void
ServoMotor_Home(void)
{
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 50); // 接近0度
}

// 预设模式1：舵机在0度和180度之间来回摆动
void
ServoMotor_Mode1(void)
{
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 250); // 接近180度
    osDelay(1200);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 50); // 接近0度
    osDelay(1200);
}

// 预设模式2：舵机在0度和90度之间来回摆动
void
ServoMotor_Mode2(void)
{
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 150); // 接近90度
    osDelay(1200);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 50); // 接近0度
    osDelay(1200);
}