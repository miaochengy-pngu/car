#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 双路数字红外 + L298N 的简单验收版本。
 *
 * 白底 = 0
 * 黑线 = 1
 */
#define TRACK_BLACK_LEVEL        1

/*
 * 小车居中时黑线位于两探头之间：
 *   00 -> 直行
 *   10 -> 左修正
 *   01 -> 右修正
 */
#define TRACK_CENTER_PATTERN     0

/*
 * 直线左右轮分开设定。
 *
 * 没有编码器时，左右电机即使给同一个 PWM，实际转速也可能不同。
 * 所以不要强制 70/70；通过这两个参数做静态补偿。
 *
 * 如果车直线向左偏：说明右轮相对更快，
 *   优先减小 SPEED_STRAIGHT_RIGHT。
 *
 * 如果车直线向右偏：说明左轮相对更快，
 *   优先减小 SPEED_STRAIGHT_LEFT。
 *
 * 当前 PWM 约 5% 一档，建议每次改 5。
 */
#define SPEED_STRAIGHT_LEFT      70
#define SPEED_STRAIGHT_RIGHT     70

/*
 * 传感器检测到偏离后，直接用差速修正。
 */
#define TURN_INNER_SPEED         45
#define TURN_OUTER_SPEED         80

/*
 * 90°直角处短暂进入特殊状态时，
 * 按最后一次转向方向继续找线。
 */
#define RECOVER_INNER_SPEED       0
#define RECOVER_OUTER_SPEED      60

#endif
