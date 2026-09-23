#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 当前实车传感器状态：
 *   灯亮 = 0 = 白底
 *   灯灭 = 1 = 黑线
 *
 * 现在正常居中时黑线同时被两路传感器检测到，因此：
 *   11 -> 正常直行
 *   10 -> 左侧仍在黑线，向左修正
 *   01 -> 右侧仍在黑线，向右修正
 *   00 -> 两侧都离开黑线，按上一次修正方向找线
 */

/* 直行左右轮可独立调，用于补偿电机差异 */
#define RUN_LEFT_SPEED           90
#define RUN_RIGHT_SPEED          90

/* 强转：内轮反转，外轮正转 */
#define TURN_INNER_SPEED       -100
#define TURN_OUTER_SPEED        100

#endif
