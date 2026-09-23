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
 * 参考公开 STC89C52RC 双红外循迹例程：
 * 小车居中时黑线位于两探头之间，所以两个探头都看到白色：
 *   00 -> 直行
 *   10 -> 左修正
 *   01 -> 右修正
 *
 * 如果你的实车摆在线中心时两个探头实际都检测到黑线，
 * 再把这里改成 3。
 */
#define TRACK_CENTER_PATTERN     0

/*
 * PWM 百分比。
 * Timer0 使用 20 级 PWM，所以实际分辨率约 5%。
 */
#define SPEED_STRAIGHT          70

#define TURN_INNER_SPEED       45
#define TURN_OUTER_SPEED       80

/*
 * 90°直角处短暂丢线时，继续沿最后一次转向方向找线。
 * 内轮停、外轮低速转，避免直接冲出赛道。
 */
#define RECOVER_INNER_SPEED      0
#define RECOVER_OUTER_SPEED     60

#endif
