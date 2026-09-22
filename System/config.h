#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * A11 红外循迹验收版参数
 *
 * 目标：
 * 1. 完成时间
 * 2. 循迹精度（尽量不压线）
 * 3. 稳定性（尽量不蛇形震荡）
 */

/* 手册确认：白底=0，黑线=1 */
#define TRACK_BLACK_LEVEL       1

/*
 * 两个探头“居中”时的状态：
 * 3 = 11：两个探头都在黑线上
 * 0 = 00：黑线位于两个探头之间
 *
 * 3 cm 黑线默认先用 11。
 * 如果实车居中时两个指示灯都是亮的，把 3 改成 0。
 */
#define TRACK_CENTER_PATTERN    3

/* 离散误差幅值 */
#define TRACK_ERROR             100

/*
 * PD 参数，数值放大 100 倍保存：
 * Kp = 0.22
 * Ki = 0
 * Kd = 0.12
 *
 * PID 模块保留完整 I 项，但本次简单验收先不用积分。
 */
#define STEER_KP_X100           22
#define STEER_KI_X100            0
#define STEER_KD_X100           12

#define STEER_INTEGRAL_LIMIT   300
#define STEER_OUTPUT_LIMIT      38

/*
 * PWM 百分比。
 * 直线较快，检测到偏线时自动降一点速度。
 */
#define SPEED_STRAIGHT           72
#define SPEED_TURN               58

/*
 * 完全丢线/特殊状态时，按最近一次偏移方向低速找回。
 */
#define RECOVER_INNER_SPEED      24
#define RECOVER_OUTER_SPEED      52

#endif
