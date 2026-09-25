#ifndef __TRACKING_H__
#define __TRACKING_H__

#define TRACK_PATTERN_BOTH_WHITE   0
#define TRACK_PATTERN_RIGHT_BLACK  1
#define TRACK_PATTERN_LEFT_BLACK   2
#define TRACK_PATTERN_BOTH_BLACK   3

void tracking_init(void);

/* 内侧两个传感器：普通循迹 */
unsigned char tracking_read_pattern(void);

/* 外侧两个传感器：90度转弯触发 */
unsigned char tracking_outer_left(void);
unsigned char tracking_outer_right(void);

#endif
