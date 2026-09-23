#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 双路数字红外 + L298N。
 *
 * 黑线 -> 灯灭 -> 1
 * 白底 -> 灯亮 -> 0
 */
#define TRACK_BLACK_LEVEL          1

/*
 * 直线左右轮独立调节，用来补偿两台电机实际转速差。
 * 当前软件 PWM 分辨率约 5%。
 */
#define SPEED_STRAIGHT_LEFT       90
#define SPEED_STRAIGHT_RIGHT      90

/*
 * 转弯：
 * 左转 = 左轮反转、右轮正转
 * 右转 = 左轮正转、右轮反转
 */
#define TURN_INNER_SPEED         -80
#define TURN_OUTER_SPEED         100

/*
 * 传感器/转向状态机参数。
 *
 * SENSOR_CONFIRM_MS:
 *   原始 00/01/10/11 连续稳定多久才承认，过滤短暂杂色误判。
 *
 * TURN_MIN_MS:
 *   一旦进入左转/右转，至少保持这么久；
 *   这段时间内绝不因为相反传感器瞬态而反向。
 *
 * CENTER_STABLE_MS:
 *   转弯后必须稳定看到 00（两灯都亮）这么久，才退出转弯状态。
 *
 * 这些参数都应小于 255 ms，因为使用 8 位毫秒计数。
 */
#define SENSOR_CONFIRM_MS         10
#define TURN_MIN_MS               80
#define CENTER_STABLE_MS          25

#endif
