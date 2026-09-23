#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 当前采用最简单的两路数字循迹：
 *   00 -> 直行
 *   11 -> 直行
 *   10 -> 左转
 *   01 -> 右转
 *
 * 这样先不依赖传感器指示灯与 MCU 电平的对应关系，
 * 避免上电后因为中心状态判断反了而一直停车。
 */

#define RUN_LEFT_SPEED           90
#define RUN_RIGHT_SPEED          90

#define TURN_INNER_SPEED       -100
#define TURN_OUTER_SPEED        100

#endif
