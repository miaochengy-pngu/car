# STC89C52RC 红外循迹小车

当前版本专门按课程验收整理：

- 完成时间
- 循迹精度（是否压线）
- 稳定性（是否频繁震荡）
- 每组重复 2 轮取平均

不再使用 PID。当前控制器是更适合“双路数字红外”的三档差速闭环：

```text
双路红外
   ↓
居中 / 左偏 / 右偏
   ↓
直行 / 柔和差速 / 强差速
   ↓
左右 PWM
   ↓
L298N
```

## 工程结构

```text
car/
├─ Project/
│  └─ car.uvproj
├─ User/
│  └─ main.c
├─ Control/
│  ├─ line_control.c
│  └─ line_control.h
├─ Hardware/
│  ├─ Motor/
│  │  ├─ motor.c
│  │  └─ motor.h
│  ├─ Tracking/
│  │  ├─ tracking.c
│  │  └─ tracking.h
│  └─ PWM/
│     ├─ pwm.c
│     └─ pwm.h
└─ System/
   ├─ config.h
   ├─ delay.c
   └─ delay.h
```

双击 `Project/car.uvproj` 打开 Keil 工程。

## 红外状态

模块逻辑：

```text
白底 -> 0 -> 指示灯亮
黑线 -> 1 -> 指示灯灭
```

编码：

```text
00 = 两个探头都白
01 = 右探头黑
10 = 左探头黑
11 = 两个探头都黑
```

当前默认：

```c
#define TRACK_CENTER_PATTERN 3
```

即居中时为 `11`。

如果实车居中时两个循迹指示灯都是亮的，则改成：

```c
#define TRACK_CENTER_PATTERN 0
```

## 三档差速

参数全部在 `System/config.h`：

```c
#define SPEED_STRAIGHT            72

#define TURN_SOFT_INNER           56
#define TURN_SOFT_OUTER           80

#define TURN_HARD_INNER           36
#define TURN_HARD_OUTER           84

#define HARD_TURN_COUNT            4

#define RECOVER_INNER_SPEED       24
#define RECOVER_OUTER_SPEED       52
```

控制逻辑：

```text
居中：
72 / 72

刚左偏：
56 / 80

连续左偏约 20 ms：
36 / 84

刚右偏：
80 / 56

连续右偏约 20 ms：
84 / 36
```

所以它仍然是闭环控制，只是不使用 PID。

## 调参原则

第一次先以“稳定完整跑完”为目标，不追求最快。

### 1. 先调直线速度

只改：

```c
SPEED_STRAIGHT
```

建议顺序：

```text
64 -> 68 -> 72 -> 76 -> 80
```

每组都跑 2 轮并记录平均时间。

如果直线开始明显蛇形或进弯来不及修正，就退回上一档。

### 2. 如果左右摆动很频繁

说明柔和修正太猛。

把：

```text
56 / 80
```

收窄，例如：

```text
60 / 76
```

即提高 `TURN_SOFT_INNER`、降低 `TURN_SOFT_OUTER`。

### 3. 如果弯道容易压线

说明持续偏离后的修正不够。

优先把强修正加大，例如：

```text
36 / 84
-> 32 / 88
-> 28 / 92
```

不要先把柔和修正也调得很猛，否则直线容易蛇形。

### 4. 如果转弯反应太慢

先减小：

```c
HARD_TURN_COUNT
```

例如：

```text
4 -> 3
```

控制周期约 5 ms，所以 4 次约 20 ms，3 次约 15 ms。

### 5. 如果一碰线就突然猛转

把：

```c
HARD_TURN_COUNT
```

增大，例如：

```text
4 -> 5 -> 6
```

让柔和修正持续更久。

### 6. 如果彻底冲出线后回不来

再调：

```c
RECOVER_INNER_SPEED
RECOVER_OUTER_SPEED
```

先保持外轮 52 左右，只逐步降低内轮，例如：

```text
24 / 52
20 / 52
16 / 52
```

让找线转向更强。

## 推荐验收调参记录

每次只改一组参数，并记录：

```text
参数组
第 1 轮时间
第 2 轮时间
平均时间
是否压线
是否明显震荡
是否丢线
```

这样最终选择的是“两轮平均最好且稳定”的参数，而不是单圈偶然最快的参数。

## PWM

Timer0 软件 PWM：

```text
PWM 频率约 200 Hz
控制更新约 200 Hz
PWM 分辨率约 4%
```

因此速度参数最好优先使用 4 的倍数。

## 接线

### L298N

| L298N | C51 主控板 | MCU |
|---|---|---|
| ENB | P10 | P1.0 |
| IN4 | P11 | P1.1 |
| IN3 | P12 | P1.2 |
| IN2 | P13 | P1.3 |
| IN1 | P14 | P1.4 |
| ENA | P15 | P1.5 |

### 电机

```text
左电机红线 -> OUT2
左电机黑线 -> OUT1

右电机红线 -> OUT4
右电机黑线 -> OUT3
```

### 红外

```text
OTL -> P3.5
OTR -> P3.4
GND -> GND
VCC -> VCC
```
