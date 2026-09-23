# STC89C52RC 红外循迹小车

当前版本使用最简单的双红外实时循迹逻辑，并按实车传感器状态适配 A11 主控板。

## 当前控制逻辑

实车已确认：

```text
灯亮 = 0 = 白底
灯灭 = 1 = 黑线
```

现在正常循迹时黑线位于传感器检测区域内，两灯都灭，因此：

```text
11 两灯灭
-> 正常直行

10 左灯灭、右灯亮
-> 左转
-> 左轮反转，右轮正转

01 左灯亮、右灯灭
-> 右转
-> 左轮正转，右轮反转

00 两灯亮
-> 丢线
-> 按上一次修正方向继续找线
```

如果上电后直接就是 00、还没有任何历史方向，则停车。

这里不使用 PID、滤波、延时锁定或复杂状态机。

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
