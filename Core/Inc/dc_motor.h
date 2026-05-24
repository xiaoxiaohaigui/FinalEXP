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
#define DCMOTOR_DEFAULT_DUTY_PERCENT 10U
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

#ifdef __cplusplus
}
#endif

#endif /* __DC_MOTOR_H__ */
