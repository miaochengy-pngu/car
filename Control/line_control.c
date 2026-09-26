#include "config.h"
#include "motor.h"
#include "tracking.h"
#include "line_control.h"
#include "timer.h"

/*
 * 状态机：
 *
 * FOLLOW_LINE
 *   - 内侧传感器持续正常循迹
 *   - 只有外侧从“亮亮(00)”变成“亮灭(01)”时触发右转
 *   - 对称地，“亮亮(00)”变成“灭亮(10)”时触发左转
 *
 * TURN_RIGHT / TURN_LEFT
 *   - 锁定 1000 ms
 *   - 期间只输出固定转弯命令
 *   - 不读取内侧/外侧传感器，不允许其它状态打断
 *
 * OUTER_LOCKOUT
 *   - 转弯结束后的 500 ms
 *   - 内侧传感器立即恢复正常循迹
 *   - 完全忽略外侧传感器，不允许再次触发弯道
 *
 * 500 ms 后回到 FOLLOW_LINE，循环往复。
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

/*
 * 外侧传感器上一帧状态。
 * 0 = 亮/白底，1 = 灭/黑线。
 * 用它实现“亮亮 -> 亮灭/灭亮”的边沿触发，而不是电平持续触发。
 */
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

static void turn_left(void)
{
    motor_set(TURN_INNER_SPEED, TURN_OUTER_SPEED);
}

static void turn_right(void)
{
    motor_set(TURN_OUTER_SPEED, TURN_INNER_SPEED);
}

/* 只使用内侧两个传感器进行普通循迹。 */
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
        turn_left();
    }
    else if (pattern == TRACK_PATTERN_RIGHT_BLACK)
    {
        turn_right();
    }
    else
    {
        /*
         * 两个内侧都没有检测到黑线时，沿用当前简单验收策略：
         * 继续向前，不在这里引入新的恢复状态。
         */
        run_forward();
    }
}

void line_control_init(void)
{
    motor_stop();

    state = FOLLOW_LINE;
    state_start_time = timer1_get_ms();

    /* 上电时记录真实外侧状态，避免一上电就误认为出现边沿。 */
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
             * 正常阶段：内侧传感器始终负责循迹。
             */
            follow_line();

            outer_left = tracking_outer_left();
            outer_right = tracking_outer_right();

            /*
             * 只接受从“亮亮(00)”出发的单侧边沿：
             *
             *   00 -> 01：外右由亮变灭，锁定右转 1 s
             *   00 -> 10：外左由亮变灭，锁定左转 1 s
             *
             * 11 不触发；持续保持 01/10 也不会重复触发。
             */
            if ((prev_outer_left == 0) && (prev_outer_right == 0))
            {
                if ((outer_left == 0) && (outer_right == 1))
                {
                    state = TURN_RIGHT_STATE;
                    state_start_time = now;
                    turn_right();
                }
                else if ((outer_left == 1) && (outer_right == 0))
                {
                    state = TURN_LEFT_STATE;
                    state_start_time = now;
                    turn_left();
                }
            }

            prev_outer_left = outer_left;
            prev_outer_right = outer_right;
            break;

        case TURN_LEFT_STATE:
            /*
             * 锁定状态：1 s 内唯一动作就是左转。
             * 不读取任何传感器，因此任何新信号都不能打断它。
             */
            turn_left();

            if (elapsed_ms(now, state_start_time) >= TURN_DURATION_MS)
            {
                state = OUTER_LOCKOUT_STATE;
                state_start_time = now;

                /* 1 s 一结束，立即恢复内侧循迹。 */
                follow_line();
            }
            break;

        case TURN_RIGHT_STATE:
            /*
             * 锁定状态：1 s 内唯一动作就是右转。
             * 不读取任何传感器，因此任何新信号都不能打断它。
             */
            turn_right();

            if (elapsed_ms(now, state_start_time) >= TURN_DURATION_MS)
            {
                state = OUTER_LOCKOUT_STATE;
                state_start_time = now;

                /* 1 s 一结束，立即恢复内侧循迹。 */
                follow_line();
            }
            break;

        case OUTER_LOCKOUT_STATE:
            /*
             * 这 500 ms 只做内侧循迹。
             * 故意不读取、不判断外侧传感器。
             */
            follow_line();

            if (elapsed_ms(now, state_start_time) >= OUTER_LOCKOUT_MS)
            {
                /*
                 * 冷却结束时才重新采样外侧，作为新的基准。
                 * 如果此时外侧仍压着黑线，不会立刻二次触发；
                 * 必须先重新回到亮亮(00)，再出现下一次边沿。
                 */
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
