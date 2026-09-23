#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 双路数字红外 + L298N。
 *
 * 黑线 -> 灯灭 -> 1
 * 白底 -> 灯亮 -> 0
 */
#define TRACK_BLACK_LEVEL          1

/*
 * 直线左右轮独立调节，用来补偿两台电机实际转速差。
 * 当前软件 PWM 分辨率约 5%。
 */
#define SPEED_STRAIGHT_LEFT       90
#define SPEED_STRAIGHT_RIGHT      90

/*
 * 转弯采用开源 STC89C52RC 循迹例程里常见的“直接动作”结构：
 *
 * 左转：左轮反转，右轮正转
 * 右转：左轮正转，右轮反转
 *
 * 你这台车低占空比不容易起转，所以默认给得比较高。
 */
#define TURN_INNER_SPEED         -80
#define TURN_OUTER_SPEED         100

#endif
