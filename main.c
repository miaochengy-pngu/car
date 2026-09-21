/*
 * STC89C52RC 双路红外循迹小车
 *
 * 硬件：
 *   - STC89C52RC
 *   - 双路 TCRT5000/LM393 数字循迹板（OTL/OTR/GND/VCC）
 *   - L298N 双 H 桥电机驱动
 *   - 两个 TT 直流减速电机
 *
 * Keil C51:
 *   - 将本文件加入工程
 *   - 使用 REG52.H
 *   - 勾选 Create HEX File
 *
 * 默认逻辑：
 *   白底 = 0
 *   黑线 = 1
 *
 * 默认接线：
 *   OTL -> P3.3
 *   OTR -> P3.2
 *   IN1 -> P1.2
 *   IN2 -> P1.3
 *   IN3 -> P1.6
 *   IN4 -> P1.7
 */

#include <REG52.H>

/* =========================
 * 1. 引脚定义
 * ========================= */

/* 双路循迹传感器 */
sbit TRACK_LEFT  = P3^3;   /* OTL */
sbit TRACK_RIGHT = P3^2;   /* OTR */

/*
 * L298N
 *
 * 左电机：IN1 / IN2
 * 右电机：IN3 / IN4
 *
 * 下面的正反转极性参考常见两轮小车安装方向：
 * 左右电机物理安装方向相反，因此“整车前进”时两侧 H 桥逻辑不同。
 */
sbit MOTOR_L_IN1 = P1^2;
sbit MOTOR_L_IN2 = P1^3;
sbit MOTOR_R_IN1 = P1^6;
sbit MOTOR_R_IN2 = P1^7;


/* =========================
 * 2. 可调参数
 * ========================= */

/*
 * 与本套双路 TCRT5000 + LM393 模块公开资料一致：
 * 黑色反射弱 -> 输出高电平 1
 * 白色反射强 -> 输出低电平 0
 *
 * 若你的实测恰好相反，把 1 改成 0。
 */
#define TRACK_BLACK_LEVEL 1

/* 转弯方式 */
#define TURN_STYLE_ARC   0   /* 一侧停、一侧前进，比较平滑 */
#define TURN_STYLE_SPIN  1   /* 一侧倒转、一侧前进，急转弯更强 */

#define TURN_STYLE TURN_STYLE_ARC

/*
 * 两个传感器同时检测到黑色时：
 * 1 = 停车，可把横向黑线作为终点线
 * 0 = 继续直行
 */
#define STOP_ON_BOTH_BLACK 1

/*
 * 主循环采样间隔。
 * 双路数字循迹不需要很长延时，2 ms 足够灵敏。
 */
#define CONTROL_PERIOD_MS 2


/* =========================
 * 3. 延时
 * ========================= */

/*
 * 约 1 ms 延时，按常见 11.0592 MHz / 12T 8051 编写。
 * 如果板上是 12 MHz，误差只影响等待时间，不影响循迹逻辑。
 */
void delay_1ms(void)
{
    unsigned char i;
    unsigned char j;

    i = 2;
    j = 199;

    do
    {
        while (--j);
    } while (--i);
}

void delay_ms(unsigned int ms)
{
    while (ms--)
    {
        delay_1ms();
    }
}


/* =========================
 * 4. 单电机底层控制
 * ========================= */

void left_motor_forward(void)
{
    MOTOR_L_IN1 = 0;
    MOTOR_L_IN2 = 1;
}

void left_motor_reverse(void)
{
    MOTOR_L_IN1 = 1;
    MOTOR_L_IN2 = 0;
}

void left_motor_stop(void)
{
    MOTOR_L_IN1 = 0;
    MOTOR_L_IN2 = 0;
}

void right_motor_forward(void)
{
    MOTOR_R_IN1 = 1;
    MOTOR_R_IN2 = 0;
}

void right_motor_reverse(void)
{
    MOTOR_R_IN1 = 0;
    MOTOR_R_IN2 = 1;
}

void right_motor_stop(void)
{
    MOTOR_R_IN1 = 0;
    MOTOR_R_IN2 = 0;
}


/* =========================
 * 5. 整车动作
 * ========================= */

void car_stop(void)
{
    left_motor_stop();
    right_motor_stop();
}

void car_forward(void)
{
    left_motor_forward();
    right_motor_forward();
}

void car_backward(void)
{
    left_motor_reverse();
    right_motor_reverse();
}

void car_turn_left(void)
{
#if TURN_STYLE == TURN_STYLE_SPIN
    /* 急转：左轮后退，右轮前进 */
    left_motor_reverse();
    right_motor_forward();
#else
    /* 平滑修正：左轮停，右轮前进 */
    left_motor_stop();
    right_motor_forward();
#endif
}

void car_turn_right(void)
{
#if TURN_STYLE == TURN_STYLE_SPIN
    /* 急转：左轮前进，右轮后退 */
    left_motor_forward();
    right_motor_reverse();
#else
    /* 平滑修正：左轮前进，右轮停 */
    left_motor_forward();
    right_motor_stop();
#endif
}


/* =========================
 * 6. 循迹输入
 * ========================= */

unsigned char is_black(unsigned char level)
{
#if TRACK_BLACK_LEVEL == 1
    return level ? 1 : 0;
#else
    return level ? 0 : 1;
#endif
}


/* =========================
 * 7. 一次循迹控制
 * ========================= */

void tracking_control(void)
{
    unsigned char left_black;
    unsigned char right_black;

    left_black  = is_black(TRACK_LEFT);
    right_black = is_black(TRACK_RIGHT);

    /*
     * 传感器安装方式：
     * 两个探头分居黑线左右，正常居中时两个探头都看到白底。
     *
     * 左探头压到黑线：说明车偏到黑线右侧，需要向左修正。
     * 右探头压到黑线：说明车偏到黑线左侧，需要向右修正。
     */

    if ((left_black == 0) && (right_black == 0))
    {
        car_forward();
    }
    else if ((left_black == 1) && (right_black == 0))
    {
        car_turn_left();
    }
    else if ((left_black == 0) && (right_black == 1))
    {
        car_turn_right();
    }
    else
    {
#if STOP_ON_BOTH_BLACK
        car_stop();
#else
        car_forward();
#endif
    }
}


/* =========================
 * 8. 主程序
 * ========================= */

void main(void)
{
    /*
     * 8051 P1/P3 为准双向口。
     * 作为输入使用时先写 1，释放端口，再读取外部电平。
     */
    TRACK_LEFT  = 1;
    TRACK_RIGHT = 1;

    car_stop();

    /*
     * 上电后留一点时间放稳小车、让传感器和电源稳定。
     */
    delay_ms(800);

    while (1)
    {
        tracking_control();
        delay_ms(CONTROL_PERIOD_MS);
    }
}
