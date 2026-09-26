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

/*
 * 正常循迹速度。
 * 直行：80 / 80
 * 小幅左修正：40 / 80
 * 小幅右修正：80 / 40
 *
 * 这些数值现在通过 Timer0 软件 PWM 真正作为占空比使用。
 */
#define RUN_LEFT_SPEED           80
#define RUN_RIGHT_SPEED          80
#define TRACK_INNER_SPEED        40
#define TRACK_OUTER_SPEED        80

/*
 * 只有外侧传感器触发 90 度弯道状态时才使用强转弯：
 * 左转：-100 / +100
 * 右转：+100 / -100
 */
#define CORNER_INNER_SPEED     -100
#define CORNER_OUTER_SPEED      100

/*
 * 外侧弯道状态机时间。
 * Timer1 提供约 1 ms 系统时基（11.0592 MHz、12T）。
 */
#define TURN_DURATION_MS       1000
#define OUTER_LOCKOUT_MS        500

#endif
