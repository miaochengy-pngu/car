/*
 * STC89C52RC 双路红外循迹小车
 *
 * 已按用户找到的原厂/商品原理图和实物 PCB 丝印修正引脚：
 *
 * 主控板“电机控制”6 针排针（从 PCB 丝印可见）：
 *   ENB -> P1.0
 *   IN4 -> P1.1
 *   IN3 -> P1.2
 *   IN2 -> P1.3
 *   IN1 -> P1.4
 *   ENA -> P1.5
 *
 * 主控板白色“红外循迹接口”：
 *   OTL -> P3.5
 *   OTR -> P3.4
 *   GND -> GND
 *   VCC -> VCC
 *
 * L298N：
 *   ENA/IN1/IN2 控制左电机
 *   ENB/IN3/IN4 控制右电机
 *
 * Keil C51:
 *   - 将本文件加入工程
 *   - 使用 REG52.H
 *   - 勾选 Create HEX File
 */

#include <REG52.H>

/* =========================
 * 1. 引脚定义
 * ========================= */

/*
 * 原厂主控板白色“红外循迹接口”对应 P3.4/P3.5。
 * 按原厂模块 OTL/OTR 与接口排列，默认：
 *   OTL(左) -> P3.5
 *   OTR(右) -> P3.4
 *
 * 若实车测试发现左右修正完全相反，只需交换下面两行。
 */
sbit TRACK_LEFT  = P3^5;   /* OTL */
sbit TRACK_RIGHT = P3^4;   /* OTR */

/* 原厂主控板“电机控制”6 针排针 */
sbit MOTOR_R_EN  = P1^0;   /* ENB */
sbit MOTOR_R_IN4 = P1^1;   /* IN4 */
sbit MOTOR_R_IN3 = P1^2;   /* IN3 */
sbit MOTOR_L_IN2 = P1^3;   /* IN2 */
sbit MOTOR_L_IN1 = P1^4;   /* IN1 */
sbit MOTOR_L_EN  = P1^5;   /* ENA */


/* =========================
 * 2. 可调参数
 * ========================= */

/*
 * 本套双路红外循迹模块：
 * 指示灯“到黑线灭、非黑线亮”。
 * 对应常见比较器输出逻辑默认：
 *   白底 = 0
 *   黑线 = 1
 *
 * 若实测数字输出相反，把 1 改成 0。
 */
#define TRACK_BLACK_LEVEL 1

#define TURN_STYLE_ARC   0
#define TURN_STYLE_SPIN  1
#define TURN_STYLE       TURN_STYLE_ARC

/* 两个探头同时检测到黑线时是否停车 */
#define STOP_ON_BOTH_BLACK 1

#define CONTROL_PERIOD_MS 2


/* =========================
 * 3. 延时
 * ========================= */

/* 约 1 ms，主板晶振 11.0592 MHz */
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
 * 4. 电机底层控制
 * ========================= */

void motor_enable(void)
{
    MOTOR_L_EN = 1;
    MOTOR_R_EN = 1;
}

void motor_disable(void)
{
    MOTOR_L_EN = 0;
    MOTOR_R_EN = 0;
}

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
    MOTOR_R_IN3 = 1;
    MOTOR_R_IN4 = 0;
}

void right_motor_reverse(void)
{
    MOTOR_R_IN3 = 0;
    MOTOR_R_IN4 = 1;
}

void right_motor_stop(void)
{
    MOTOR_R_IN3 = 0;
    MOTOR_R_IN4 = 0;
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
    left_motor_reverse();
    right_motor_forward();
#else
    left_motor_stop();
    right_motor_forward();
#endif
}

void car_turn_right(void)
{
#if TURN_STYLE == TURN_STYLE_SPIN
    left_motor_forward();
    right_motor_reverse();
#else
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
     * 8051 P3 作为输入时先写 1 释放端口。
     */
    TRACK_LEFT  = 1;
    TRACK_RIGHT = 1;

    motor_enable();
    car_stop();

    /* 上电稳定时间 */
    delay_ms(800);

    while (1)
    {
        tracking_control();
        delay_ms(CONTROL_PERIOD_MS);
    }
}
