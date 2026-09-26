#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 四路数字红外：
 *   内左  P3.5、内右 P3.4：正常循迹
 *   外左  P3.6、外右 P3.7：90度弯道触发
 *
 * 当前硬件约定：
 *   白底 = 0 = 指示灯亮
 *   黑线 = 1 = 指示灯灭
 */

#define RUN_LEFT_SPEED          100
#define RUN_RIGHT_SPEED         100

/*
 * 内侧循迹和锁定转弯当前共用这组方向命令。
 * 左转：左轮反转、右轮正转。
 * 右转：左轮正转、右轮反转。
 */
#define TURN_INNER_SPEED       -100
#define TURN_OUTER_SPEED        100

/*
 * 外侧弯道状态机时间。
 * Timer1 提供约 1 ms 系统时基（11.0592 MHz、12T）。
 */
#define TURN_DURATION_MS       1000
#define OUTER_LOCKOUT_MS        500

#endif
