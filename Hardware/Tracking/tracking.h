#ifndef __TRACKING_H__
#define __TRACKING_H__

#define TRACK_PATTERN_BOTH_WHITE  0
#define TRACK_PATTERN_RIGHT_BLACK 1
#define TRACK_PATTERN_LEFT_BLACK  2
#define TRACK_PATTERN_BOTH_BLACK  3

void tracking_init(void);
void tracking_calibrate_center(void);

unsigned char tracking_read_pattern(void);
unsigned char tracking_get_center_pattern(void);

#endif
