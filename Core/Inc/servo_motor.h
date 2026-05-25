#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

void ServoMotor_Init(void); // 初始化舵机
void ServoMotor_Home(void); // 舵机归零
void ServoMotor_Mode1(void); // 预设模式 1：舵机在 0 度和 180 度之间来回摆动
void ServoMotor_Mode2(void); // 预设模式 2：舵机在 0 度和 90 度之间来回摆动

#endif // SERVO_MOTOR_H