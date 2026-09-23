# STC89C52RC 红外循迹小车

当前版本按公开 STC89C52RC 循迹小车的常见结构重新整理：

```text
Timer0 中断
   ↓
只产生左右电机 PWM

main while(1)
   ↓
不断读取两路红外
   ↓
直接更新左右 PWM 占空比
```

PWM 不在 `while(1)` 里通过延时生成，所以主循环不会因为“做 PWM”而阻塞。

## 为什么这样写

已核对公开的 STC89C52RC + 11.0592 MHz 循迹小车例程，其中典型写法就是：

```text
Timer0: 约 1 ms 中断
pwm_counter++
根据 duty 决定 PWM IO 高/低

while(1):
    读左右循迹传感器
    直行 / 左转 / 右转
    只修改左右 duty
```

当前工程保留这个结构，但按照你的 A11 板实际引脚重写，没有照搬别人的引脚。

## 当前循迹逻辑

默认：

```text
00 -> 居中，70 / 70
10 -> 左偏，45 / 80
01 -> 右偏，80 / 45
11 -> 按上一次转向方向找线
```

90°直角处如果短暂丢线：

```text
刚才向左 -> 0 / 60
刚才向右 -> 60 / 0
```

## 关键参数

都在：

```text
System/config.h
```

当前：

```c
#define TRACK_CENTER_PATTERN     0

#define SPEED_STRAIGHT          70
#define TURN_INNER_SPEED       45
#define TURN_OUTER_SPEED       80

#define RECOVER_INNER_SPEED      0
#define RECOVER_OUTER_SPEED     60
```

## 调参

如果直线稳定但太慢：

```text
SPEED_STRAIGHT
70 -> 75 -> 80
```

如果左右蛇形：

```text
45 / 80
-> 50 / 75
-> 55 / 75
```

减小左右差速。

如果 90°直角拐不过去：

```text
45 / 80
-> 35 / 85
```

仍然不够时，主要加强丢线找线：

```text
0 / 60
-> 0 / 70
```

## PWM

Timer0 约每 1 ms 中断一次。

```text
20 级 PWM
PWM 周期约 20 ms
PWM 频率约 50 Hz
PWM 分辨率约 5%
```

中断函数只做：

```text
重装 TH0/TL0
PWM counter +1
更新 ENA
更新 ENB
```

不在中断中执行循迹判断、延时或复杂状态机。

## 红外中心状态必须先确认

公开双红外例程通常采用：

```text
00 = 正常居中
10 = 左边碰线
01 = 右边碰线
```

所以当前默认：

```c
#define TRACK_CENTER_PATTERN 0
```

第一次上车时，把车摆正在线中心：

- 两个传感器都显示白底状态 -> 保持 `0`
- 两个传感器都检测到黑线 -> 改为 `3`

## 硬件

L298N 控制：

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
