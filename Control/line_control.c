#include "config.h"
#include "pwm.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"

#define STATE_STRAIGHT  0
#define STATE_LEFT      1
#define STATE_RIGHT     2

static unsigned char g_state = STATE_STRAIGHT;

/* 传感器稳定确认 */
static unsigned char g_candidate_pattern = 0;
static unsigned char g_stable_pattern = 0;
static unsigned char g_candidate_since = 0;

/* 当前转向状态开始时间 */
static unsigned char g_state_since = 0;

/* 00（两灯都亮）稳定计时 */
static unsigned char g_center_since = 0;
static unsigned char g_center_timing = 0;

static unsigned char elapsed_ms(unsigned char now, unsigned char start)
{
    /* unsigned char 自然溢出，适合 <255 ms 的短时间比较 */
    return (unsigned char)(now - start);
}

static void drive_straight(void)
{
    motor_set(SPEED_STRAIGHT_LEFT, SPEED_STRAIGHT_RIGHT);
}

static void turn_left(void)
{
    motor_set(TURN_INNER_SPEED, TURN_OUTER_SPEED);
}

static void turn_right(void)
{
    motor_set(TURN_OUTER_SPEED, TURN_INNER_SPEED);
}

/*
 * 只有某个原始状态连续保持 SENSOR_CONFIRM_MS，
 * 才更新 g_stable_pattern。
 *
 * 因此转弯时偶然扫到其它颜色形成的 1~几 ms 毛刺，
 * 不会直接成为有效状态。
 */
static void update_sensor_filter(unsigned char now)
{
    unsigned char raw_pattern;

    raw_pattern = tracking_read_pattern();

    if (raw_pattern != g_candidate_pattern)
    {
        g_candidate_pattern = raw_pattern;
        g_candidate_since = now;
        return;
    }

    if (g_stable_pattern != g_candidate_pattern)
    {
        if (elapsed_ms(now, g_candidate_since) >= SENSOR_CONFIRM_MS)
        {
            g_stable_pattern = g_candidate_pattern;
        }
    }
}

void line_control_init(void)
{
    unsigned char now;
    unsigned char pattern;

    now = pwm_millis8();
    pattern = tracking_read_pattern();

    g_state = STATE_STRAIGHT;

    g_candidate_pattern = pattern;
    g_stable_pattern = pattern;
    g_candidate_since = now;

    g_state_since = now;
    g_center_since = now;
    g_center_timing = 0;

    motor_stop();
}

void line_control_step(void)
{
    unsigned char now;

    now = pwm_millis8();
    update_sensor_filter(now);

    /*
     * =========================
     * 直行
     * =========================
     */
    if (g_state == STATE_STRAIGHT)
    {
        drive_straight();

        if (g_stable_pattern == TRACK_PATTERN_LEFT_BLACK)
        {
            g_state = STATE_LEFT;
            g_state_since = now;
            g_center_timing = 0;
            turn_left();
        }
        else if (g_stable_pattern == TRACK_PATTERN_RIGHT_BLACK)
        {
            g_state = STATE_RIGHT;
            g_state_since = now;
            g_center_timing = 0;
            turn_right();
        }

        /*
         * 11 在只有两个数字传感器时无法判断左右。
         * 直行状态遇到 11 时不凭空选方向，继续当前直行命令。
         */
        return;
    }

    /*
     * =========================
     * 左转
     * =========================
     */
    if (g_state == STATE_LEFT)
    {
        /*
         * 锁定左转。
         * 即使转弯过程中误读到 01，也绝不直接切成右转。
         */
        turn_left();

        if (elapsed_ms(now, g_state_since) < TURN_MIN_MS)
        {
            g_center_timing = 0;
            return;
        }

        /*
         * 最短转向时间以后，只有稳定回到 00 才允许结束左转。
         */
        if (g_stable_pattern == TRACK_PATTERN_BOTH_WHITE)
        {
            if (!g_center_timing)
            {
                g_center_timing = 1;
                g_center_since = now;
            }
            else if (elapsed_ms(now, g_center_since) >= CENTER_STABLE_MS)
            {
                g_state = STATE_STRAIGHT;
                g_center_timing = 0;
                drive_straight();
            }
        }
        else
        {
            g_center_timing = 0;
        }

        return;
    }

    /*
     * =========================
     * 右转
     * =========================
     */
    if (g_state == STATE_RIGHT)
    {
        /*
         * 锁定右转。
         * 即使转弯过程中误读到 10，也绝不直接切成左转。
         */
        turn_right();

        if (elapsed_ms(now, g_state_since) < TURN_MIN_MS)
        {
            g_center_timing = 0;
            return;
        }

        if (g_stable_pattern == TRACK_PATTERN_BOTH_WHITE)
        {
            if (!g_center_timing)
            {
                g_center_timing = 1;
                g_center_since = now;
            }
            else if (elapsed_ms(now, g_center_since) >= CENTER_STABLE_MS)
            {
                g_state = STATE_STRAIGHT;
                g_center_timing = 0;
                drive_straight();
            }
        }
        else
        {
            g_center_timing = 0;
        }

        return;
    }

    /* 防御性恢复 */
    g_state = STATE_STRAIGHT;
    g_center_timing = 0;
    drive_straight();
}
