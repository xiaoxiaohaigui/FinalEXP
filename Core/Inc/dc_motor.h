#ifndef __DC_MOTOR_H__
#define __DC_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 直流电机运行方向定义。 */
typedef enum
{
	DCMOTOR_BRAKE = 0,
	DCMOTOR_FORWARD,
	DCMOTOR_REVERSE,
} DCMotor_Direction_t;

/* 默认调速占空比，按百分比设置。 */
#ifndef DCMOTOR_DEFAULT_DUTY_PERCENT
#define DCMOTOR_DEFAULT_DUTY_PERCENT 50U
#endif


/* 初始化直流电机驱动并启动 TIM1 的 CH1/CH2 PWM。 */
void DCMotor_Init(void);
/* 按方向和占空比设置直流电机输出；占空比范围 0~100。 */
void DCMotor_Set(DCMotor_Direction_t direction, uint8_t dutyPercent);
/* 以指定占空比正转。 */
void DCMotor_Forward(uint8_t dutyPercent);
/* 以指定占空比反转。 */
void DCMotor_Reverse(uint8_t dutyPercent);
/* 两路 PWM 都拉低，实现制动。 */
void DCMotor_Brake(void);
/* 在当前方向基础上调整占空比，delta 可正可负，最终钳位在 0~100。 */
void DCMotor_AdjustDuty(int8_t delta);
/* 获取当前占空比（0~100）。 */
uint8_t DCMotor_GetDuty(void);

/* ── 编码器测速接口 ──────────────────────────────────────────── */

/**
 * @brief  将 TIM3 编码器计数器置零，并复位转速跟踪状态。
 * @note   通常在电机启动前调用，以将当前位置作为速度零点。
 *         调用本函数会自动启动编码器接口（如尚未启动）。
 */
void DCMotor_ResetEncoder(void);

/**
 * @brief  根据编码器计数值和系统时间计算当前转速（deciRPM，即 RPM × 10）。
 * @note   调用间隔不必固定——函数内部会根据 HAL_GetTick() 自动计算时间差。
 *         相邻两次调用之间的间隔建议不超过 1.6 秒（32767 个计数对应的
 *         时间窗口，3000 RPM 下安全调用间隔约为 1.6 秒，更低转速下
 *         可接受更长的调用间隔）。
 *         返回值使用整型计算，不依赖浮点库。
 *         采用 deciRPM（RPM × 10）提供 0.1 RPM 分辨率，适合低速测速场景。
 * @retval 转速 × 10，正值对应编码器计数增加的方向（电机正转），
 *         负值对应编码器计数减少的方向（电机反转）。
 *         例如：返回值 123 表示 12.3 RPM。
 */
int32_t DCMotor_GetSpeedDeciRPM(void);

#ifdef __cplusplus
}
#endif

#endif /* __DC_MOTOR_H__ */
