#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 当前先完全绕过 PWM，只看正转 / 反转 / 停止。
 *
 * 两路传感器：
 *   00 -> 直行
 *   11 -> 直行
 *   10 -> 左转
 *   01 -> 右转
 *
 * motor_set() 当前只看正负号：
 *   >0 正转
 *   <0 反转
 *    0 停止
 */
#define RUN_LEFT_SPEED          100
#define RUN_RIGHT_SPEED         100

#define TURN_INNER_SPEED       -100
#define TURN_OUTER_SPEED        100

#endif
