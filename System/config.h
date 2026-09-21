#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/* A11 C51 line-following car configuration. */

/* TCRT5000 module: white background = 0, black line = 1. */
#define TRACK_BLACK_LEVEL 1

/* Steering mode. */
#define TURN_STYLE_ARC   0
#define TURN_STYLE_SPIN  1
#define TURN_STYLE       TURN_STYLE_ARC

/* Both sensors on black: stop and treat it as a finish line. */
#define STOP_ON_BOTH_BLACK 1

/* Main control-loop period. */
#define CONTROL_PERIOD_MS 2

#endif
