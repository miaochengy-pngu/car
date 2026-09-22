#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * Competition-oriented configuration for the A11 C51 line follower.
 *
 * Sensor module:
 *   white background = 0
 *   black line       = 1
 */
#define TRACK_BLACK_LEVEL 1

/*
 * At power-on, place the car centered on the track.
 * The firmware samples the two sensors and decides whether the
 * nominal centered state is 00 or 11. This avoids hard-coding one
 * mounting geometry.
 */
#define TRACK_AUTO_CENTER_ENABLE 1
#define TRACK_CENTER_DEFAULT     0

/*
 * Steering PID gains are stored x100.
 *
 * For this two-digital-sensor car, Ki intentionally starts at 0.
 * The controller is therefore initially PD, but the complete I term
 * and anti-windup path are implemented and can be enabled later.
 */
#define STEER_KP_X100          24
#define STEER_KI_X100           0
#define STEER_KD_X100          18

#define STEER_INTEGRAL_LIMIT  500
#define STEER_OUTPUT_LIMIT     55
#define TRACK_ERROR_FULL      100

/*
 * Speed profile, in PWM percent.
 * Raise SPEED_STRAIGHT only after the sensor polarity and motor
 * directions have been verified on the real car.
 */
#define SPEED_STRAIGHT          88
#define SPEED_MIN               42
#define SPEED_MAX               96
#define SPEED_SLOWDOWN_MAX      34

/*
 * When the sensors enter the pattern opposite to the calibrated
 * center state, keep searching in the most recently observed
 * steering direction instead of driving blindly straight.
 */
#define SEARCH_FORWARD_SPEED    62
#define SEARCH_REVERSE_SPEED    26
#define SPECIAL_STRAIGHT_SPEED  55

/*
 * After leaving an edge sensor, decay the previous steering command
 * for a short time. This is useful with only two digital sensors:
 * it damps snap-back and gives a little line-loss memory.
 *
 * Control loop is 1 kHz, so this value is approximately milliseconds.
 */
#define STEERING_MEMORY_TICKS   12

#endif
