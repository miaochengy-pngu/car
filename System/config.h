#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 双路数字红外 + L298N。
 *
 * 白底 = 0
 * 黑线 = 1
 */
#define TRACK_BLACK_LEVEL          1

/*
 * 小车居中时黑线位于两探头之间：
 *   00 -> 直行
 *   10 -> 触发左转
 *   01 -> 触发右转
 *   11 -> 方向不明确
 */
#define TRACK_CENTER_PATTERN       0

/*
 * 直线左右轮分开调，用来补偿两个电机实际转速差异。
 */
#define SPEED_STRAIGHT_LEFT       70
#define SPEED_STRAIGHT_RIGHT      70

/*
 * 转弯动作。
 * TURN_INNER_SPEED 可以写负数实现内轮反转。
 *
 * 例如：
 *   左转 = inner / outer
 *   右转 = outer / inner
 */
#define TURN_INNER_SPEED         -70
#define TURN_OUTER_SPEED         100

/*
 * 触发一次左/右转后，固定维持该动作这么久。
 * 你现在要求先试 2 s，所以设为 2000 ms。
 *
 * 实车 90°弯通常 2 s 会偏长，后续可直接改成
 * 150~500 ms 一档一档测试。
 */
#define TURN_HOLD_MS            2000

#endif
