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
#define TRACK_BLACK_LEVEL          1

/*
 * 两个探头“居中”时的状态：
 * 3 = 11：两个探头都在黑线上
 * 0 = 00：黑线位于两个探头之间
 *
 * 当前先按 3 cm 黑线使用 11。
 * 如果实车居中时两个循迹指示灯都是亮的，把 3 改成 0。
 */
#define TRACK_CENTER_PATTERN       3

/*
 * 三档差速参数，单位都是 PWM 百分比。
 *
 * 直线：左右同速。
 * 刚偏线：柔和差速，避免一碰线就猛打方向导致蛇形。
 * 持续偏线：增大差速，避免弯道来不及修正而压线。
 *
 * 当前 PWM 分辨率约 4%，所以参数尽量取 4 的倍数。
 */
#define SPEED_STRAIGHT            72

#define TURN_SOFT_INNER           56
#define TURN_SOFT_OUTER           80

#define TURN_HARD_INNER           36
#define TURN_HARD_OUTER           84

/*
 * 控制器约 200 Hz，即每次约 5 ms。
 * 同一方向连续偏离达到 4 次（约 20 ms）后切换为强修正。
 */
#define HARD_TURN_COUNT            4

/*
 * 完全丢线/特殊状态时，按最近一次偏移方向低速找回。
 */
#define RECOVER_INNER_SPEED       24
#define RECOVER_OUTER_SPEED       52

#endif
