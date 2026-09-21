/*
 * STC89C52RC 双路红外循迹小车
 *
 * 依据《A套餐红外循迹组装调试指南》与实物 PCB 丝印整理。
 *
 * 主控板电机控制接口：
 *   ENB -> P1.0
 *   IN4 -> P1.1
 *   IN3 -> P1.2
 *   IN2 -> P1.3
 *   IN1 -> P1.4
 *   ENA -> P1.5
 *
 * 红外循迹接口（实物 PCB）：
 *   OTL -> P3.5
 *   OTR -> P3.4
 *   GND -> GND
 *   VCC -> VCC
 *
 * 手册给出的 L298N 正反转逻辑：
 *   左电机：ENA=1, IN1=1, IN2=0 -> 正转
 *           ENA=1, IN1=0, IN2=1 -> 反转
 *   右电机：ENB=1, IN3=1, IN4=0 -> 反转
 *           ENB=1, IN3=0, IN4=1 -> 正转
 *
 * 手册给出的循迹模块逻辑：
 *   反射强（白底）-> 输出低电平，指示灯亮
 *   反射弱（黑线）-> 输出高电平，指示灯灭
 *   因此：白底=0，黑线=1
 */

#include <REG52.H>

/* =========================
 * 1. 引脚定义
 * ========================= */

/* 双路红外循迹 */
sbit TRACK_LEFT  = P3^5;   /* OTL */
sbit TRACK_RIGHT = P3^4;   /* OTR */

/* L298N 控制 */
sbit MOTOR_R_EN  = P1^0;   /* ENB */
sbit MOTOR_R_IN4 = P1^1;   /* IN4 */
sbit MOTOR_R_IN3 = P1^2;   /* IN3 */
sbit MOTOR_L_IN2 = P1^3;   /* IN2 */
sbit MOTOR_L_IN1 = P1^4;   /* IN1 */
sbit MOTOR_L_EN  = P1^5;   /* ENA */


/* =========================
 * 2. 参数
 * ========================= */

/* 手册确认：黑线为高电平 1，白底为低电平 0 */
#define TRACK_BLACK_LEVEL 1

/* 转弯方式 */
#define TURN_STYLE_ARC   0   /* 一侧停、一侧前进 */
#define TURN_STYLE_SPIN  1   /* 一侧反转、一侧前进 */
#define TURN_STYLE       TURN_STYLE_ARC

/* 两个探头都压到黑线时停车 */
#define STOP_ON_BOTH_BLACK 1

/* 控制循环周期 */
#define CONTROL_PERIOD_MS 2


/* =========================
 * 3. 延时
 * ========================= */

/* 主板晶振为 11.0592 MHz，约 1 ms */
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

/*
 * 注意：左右电机物理安装方向相反。
 * 下列电平组合严格按套件手册的电机状态表编写。
 */

void left_motor_forward(void)
{
    /* ENA=1, IN1=1, IN2=0 -> 左电机正转 */
    MOTOR_L_IN1 = 1;
    MOTOR_L_IN2 = 0;
}

void left_motor_reverse(void)
{
    /* ENA=1, IN1=0, IN2=1 -> 左电机反转 */
    MOTOR_L_IN1 = 0;
    MOTOR_L_IN2 = 1;
}

void left_motor_stop(void)
{
    MOTOR_L_IN1 = 0;
    MOTOR_L_IN2 = 0;
}

void right_motor_forward(void)
{
    /* ENB=1, IN3=0, IN4=1 -> 右电机正转 */
    MOTOR_R_IN3 = 0;
    MOTOR_R_IN4 = 1;
}

void right_motor_reverse(void)
{
    /* ENB=1, IN3=1, IN4=0 -> 右电机反转 */
    MOTOR_R_IN3 = 1;
    MOTOR_R_IN4 = 0;
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
 * 7. 循迹控制
 * ========================= */

/*
 * 本车两探头分居黑线左右。
 *
 * 左 右
 * 0  0 : 两侧均白，黑线位于两探头之间 -> 前进
 * 1  0 : 左探头压黑线 -> 向左修正
 * 0  1 : 右探头压黑线 -> 向右修正
 * 1  1 : 两侧同时黑 -> 停车（可作为横向终点线）
 */
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
     * 8051 准双向口作为输入时先写 1，释放端口。
     */
    TRACK_LEFT  = 1;
    TRACK_RIGHT = 1;

    motor_enable();
    car_stop();

    /* 上电后等待传感器和电源稳定 */
    delay_ms(800);

    while (1)
    {
        tracking_control();
        delay_ms(CONTROL_PERIOD_MS);
    }
}
