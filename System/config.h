#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 这一版直接按用户上传的
 * “51循迹小车（利用定时器间断测距）/循迹小车 - 2/main.c”
 * 的核心循迹逻辑适配 A11 主板。
 *
 * 原例程：
 *   00 -> 直行
 *   01 -> 右转（右轮反转、左轮正转）
 *   10 -> 左转（左轮反转、右轮正转）
 *   11 -> 直行
 *
 * 原例程 PWM 很低，但你的实车已经确认低占空比电机起不来，
 * 所以这里只保留它的控制逻辑，速度按实车提高。
 */

/* 直行左右轮可独立调，用于补偿电机差异 */
#define RUN_LEFT_SPEED           90
#define RUN_RIGHT_SPEED          90

/* 强转：内轮反转，外轮正转 */
#define TURN_INNER_SPEED       -100
#define TURN_OUTER_SPEED        100

#endif
