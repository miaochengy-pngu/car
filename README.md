# STC89C52RC 红外循迹小车

当前版本直接按用户上传的开源工程：

```text
51循迹小车（利用定时器间断测距）
└─ 循迹小车 - 2
   └─ main.c
```

中的双红外循迹逻辑，适配到这块 A11 主控板。

## 当前控制逻辑

实车已经确认：

```text
灯亮 = 0 = 白底
灯灭 = 1 = 黑线/无反射
```

黑线较窄，正常位于两个传感器之间，所以：

```text
00 两灯亮
-> 直行

01 右灯灭
-> 右转
-> 左轮正转，右轮反转

10 左灯灭
-> 左转
-> 左轮反转，右轮正转

11 两灯灭
-> 直行
```

这里不使用 PID、滤波、延时锁定、历史方向或复杂状态机。

## A11 主板引脚

电机：

| L298N | MCU |
|---|---|
| ENB | P1.0 |
| IN4 | P1.1 |
| IN3 | P1.2 |
| IN2 | P1.3 |
| IN1 | P1.4 |
| ENA | P1.5 |

红外：

```text
OTL -> P3.5
OTR -> P3.4
```

## 速度参数

在：

```text
System/config.h
```

当前：

```c
#define RUN_LEFT_SPEED           90
#define RUN_RIGHT_SPEED          90

#define TURN_INNER_SPEED       -100
#define TURN_OUTER_SPEED        100
```

直线如果总往一边偏，只分别调整：

```text
RUN_LEFT_SPEED
RUN_RIGHT_SPEED
```

转弯力度则调整：

```text
TURN_INNER_SPEED
TURN_OUTER_SPEED
```

负数代表反转。

## PWM

Timer0 中断持续产生软件 PWM，主循环只读取传感器并立即更新电机命令。

当前为约 1 ms 中断、20 级 PWM，约 5% 分辨率。
