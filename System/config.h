#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 四路数字红外：
 *   内左 P3.5、内右 P3.4：正常循迹
 *   外左 P3.6、外右 P3.7：90度弯道触发
 *
 * 白底 = 0
 * 黑线 = 1
 */

/*
 * 普通循迹：降低修正幅度，避免数字传感器抖动导致左右震荡。
 * 直行：70 / 70
 * 左修正：60 / 80
 * 右修正：80 / 60
 */
#define RUN_LEFT_SPEED           70
#define RUN_RIGHT_SPEED          70
#define TRACK_INNER_SPEED        60
#define TRACK_OUTER_SPEED        80

/*
 * 仅90度弯道使用强转弯。
 */
#define CORNER_INNER_SPEED     -100
#define CORNER_OUTER_SPEED      100

#define TURN_DURATION_MS       1000
#define OUTER_LOCKOUT_MS        500

/* 普通循迹连续检测次数 */
#define TRACK_FILTER_COUNT        3

#endif
