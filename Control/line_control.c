#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"
#include "timer.h"

/*
 * 三种互相分离的动作：
 *
 * 1. 正常直行：
 *      80 / 80
 *
 * 2. 内侧传感器普通循迹修正：
 *      左修正  40 / 80
 *      右修正  80 / 40
 *    两个轮子都只正转，不允许反转。
 *
 * 3. 外侧传感器触发 90 度弯道：
 *      左转   -100 / +100
 *      右转   +100 / -100
 *    只有锁定转弯状态可以使用反转。
 */

typedef enum
{
    FOLLOW_LINE = 0,
    TURN_LEFT_STATE,
    TURN_RIGHT_STATE,
    OUTER_LOCKOUT_STATE
} CarState;

static CarState state = FOLLOW_LINE;
static unsigned int state_start_time = 0;

static unsigned char prev_outer_left = 0;
static unsigned char prev_outer_right = 0;

static unsigned int elapsed_ms(unsigned int now, unsigned int start)
{
    return (unsigned int)(now - start);
}

static void run_forward(void)
{
    motor_set(RUN_LEFT_SPEED, RUN_RIGHT_SPEED);
}

/* 内侧传感器使用的小幅循迹修正。 */
static void track_left(void)
{
    motor_set(TRACK_INNER_SPEED, TRACK_OUTER_SPEED);
}

static void track_right(void)
{
    motor_set(TRACK_OUTER_SPEED, TRACK_INNER_SPEED);
}

/* 只有外侧传感器锁定转弯状态才能调用的强转弯。 */
static void corner_left(void)
{
    motor_set(CORNER_INNER_SPEED, CORNER_OUTER_SPEED);
}

static void corner_right(void)
{
    motor_set(CORNER_OUTER_SPEED, CORNER_INNER_SPEED);
}

/* 只读取内侧两个传感器，执行普通循迹。 */
static void follow_line(void)
{
    unsigned char pattern;

    pattern = tracking_read_pattern();

    if (pattern == TRACK_PATTERN_BOTH_BLACK)
    {
        run_forward();
    }
    else if (pattern == TRACK_PATTERN_LEFT_BLACK)
    {
        track_left();
    }
    else if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        track_right();
    }
    else
    {
        run_forward();
    }
}

void line_control_init(void)
{
    motor_stop();

    state = FOLLOW_LINE;
    state_start_time = timer1_get_ms();

    prev_outer_left = tracking_outer_left();
    prev_outer_right = tracking_outer_right();
}

void line_control_step(void)
{
    unsigned int now;
    unsigned char outer_left;
    unsigned char outer_right;

    now = timer1_get_ms();

    switch (state)
    {
        case FOLLOW_LINE:
            /*
             * 正常阶段始终先由内侧传感器循迹：
             * 直行 80/80，修正只用 40/80 或 80/40。
             */
            follow_line();

            outer_left = tracking_outer_left();
            outer_right = tracking_outer_right();

            /*
             * 外侧边沿触发：
             *   00 -> 01：锁定右转
             *   00 -> 10：锁定左转
             */
            if ((prev_outer_left == 0) && (prev_outer_right == 0))
            {
                if ((outer_left == 0) && (outer_right == 1))
                {
                    state = TURN_RIGHT_STATE;
                    state_start_time = now;
                    corner_right();
                }
                else if ((outer_left == 1) && (outer_right == 0))
                {
                    state = TURN_LEFT_STATE;
                    state_start_time = now;
                    corner_left();
                }
            }

            prev_outer_left = outer_left;
            prev_outer_right = outer_right;
            break;

        case TURN_LEFT_STATE:
            /*
             * 外侧触发后的锁定状态。
             * 1 秒内只输出 -100/+100，不读取任何传感器。
             */
            corner_left();

            if (elapsed_ms(now, state_start_time) >= TURN_DURATION_MS)
            {
                state = OUTER_LOCKOUT_STATE;
                state_start_time = now;
                follow_line();
            }
            break;

        case TURN_RIGHT_STATE:
            /*
             * 外侧触发后的锁定状态。
             * 1 秒内只输出 +100/-100，不读取任何传感器。
             */
            corner_right();

            if (elapsed_ms(now, state_start_time) >= TURN_DURATION_MS)
            {
                state = OUTER_LOCKOUT_STATE;
                state_start_time = now;
                follow_line();
            }
            break;

        case OUTER_LOCKOUT_STATE:
            /*
             * 转弯后的 0.5 秒：
             * 内侧正常循迹，但完全不判断外侧传感器。
             */
            follow_line();

            if (elapsed_ms(now, state_start_time) >= OUTER_LOCKOUT_MS)
            {
                prev_outer_left = tracking_outer_left();
                prev_outer_right = tracking_outer_right();
                state = FOLLOW_LINE;
            }
            break;

        default:
            state = FOLLOW_LINE;
            prev_outer_left = tracking_outer_left();
            prev_outer_right = tracking_outer_right();
            follow_line();
            break;
    }
}
