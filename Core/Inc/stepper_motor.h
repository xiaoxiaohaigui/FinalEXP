#ifndef __STEPPER_MOTOR_H__
#define __STEPPER_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 角度校准倍率：如果实际转角与命令角不一致，只需修改这里。 */
#ifndef STEPPER_ANGLE_SCALE
#define STEPPER_ANGLE_SCALE 1.0f
#endif

/* 初始化步进电机驱动，默认关闭所有相位输出。 */
void Stepper_Init(void);
/* 立即关闭全部相位输出，使电机停止励磁。 */
void Stepper_Stop(void);
/* 按四相八拍序列正转指定步数。stepDelayUs 为每拍延时，单位微秒。 */
void Stepper_Forward(uint32_t stepCount, uint32_t stepDelayUs);
/* 按四相八拍序列反转指定步数。stepDelayUs 为每拍延时，单位微秒。 */
void Stepper_Reverse(uint32_t stepCount, uint32_t stepDelayUs);
/* 按角度旋转电机，正值正转，负值反转。 */
void Stepper_RotateAngle(float angleDeg, uint32_t stepDelayUs);

#ifdef __cplusplus
}
#endif

#endif /* __STEPPER_MOTOR_H__ */
