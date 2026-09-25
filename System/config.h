#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 四路红外循迹：
 *
 * 黑线 = 1 (传感器灯灭)
 * 白底 = 0 (传感器灯亮)
 *
 * 内侧传感器：普通循迹
 * 外侧传感器：90度弯触发
 */

#define RUN_LEFT_SPEED          100
#define RUN_RIGHT_SPEED         100

/* 原地差速转弯：内轮反转，外轮正转 */
#define TURN_INNER_SPEED       -100
#define TURN_OUTER_SPEED        100

/* 外侧传感器触发后的固定转弯时间 */
#define TURN_TIME_MS           1000

#endif
