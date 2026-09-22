# STC89C52RC 红外循迹小车

当前版本按“简单验收”整理，不再做复杂竞赛状态机。

验收重点：

- 完成时间
- 循迹精度（是否压线）
- 稳定性（是否频繁左右震荡）
- 每组重复 2 轮取平均

因此控制器采用：

```text
双路红外
   ↓
离散误差
   ↓
PD
   ↓
左右 PWM 差速
   ↓
L298N
```

其中 PID 模块仍保留完整 P/I/D，但默认 `Ki=0`，实际先按 PD 使用。

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
├─ Algorithm/
│  └─ PID/
│     ├─ pid.c
│     └─ pid.h
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

双击：

```text
Project/car.uvproj
```

即可打开 Keil C51 工程。

## 当前控制逻辑

红外模块：

```text
白底 -> 0 -> 指示灯亮
黑线 -> 1 -> 指示灯灭
```

两路状态编码：

```text
00 = 两个探头都白
01 = 右探头黑
10 = 左探头黑
11 = 两个探头都黑
```

默认按 3 cm 黑线，先设置：

```c
#define TRACK_CENTER_PATTERN 3
```

即正常居中时为 `11`。

如果你把车摆在线中央时发现两个循迹指示灯都是亮的，说明实际居中是 `00`，只需把：

```c
#define TRACK_CENTER_PATTERN 3
```

改成：

```c
#define TRACK_CENTER_PATTERN 0
```

不需要改其他代码。

## PD 差速

有效误差：

```text
10 -> error = -100 -> 向左修正
居中 -> error = 0
01 -> error = +100 -> 向右修正
```

控制器：

```text
correction = Kp * error + Kd * (error - last_error)

left_pwm  = base_speed + correction
right_pwm = base_speed - correction
```

偏线时使用较低基础速度，直线使用较高基础速度。

## 当前初始参数

全部集中在：

```text
System/config.h
```

当前值：

```c
#define STEER_KP_X100           22
#define STEER_KI_X100            0
#define STEER_KD_X100           12

#define SPEED_STRAIGHT           72
#define SPEED_TURN               58

#define RECOVER_INNER_SPEED      24
#define RECOVER_OUTER_SPEED      52
```

解释：

- `SPEED_STRAIGHT`：直接影响完成时间。
- `Kp`：越大，偏线后拉回越快。
- `Kd`：主要抑制左右来回震荡。
- `SPEED_TURN`：弯道速度，太高容易压线。
- `Ki=0`：当前验收不需要积分。

## 建议调参顺序

先保证能稳定跑完整圈，再提速。

推荐顺序：

```text
1. SPEED_STRAIGHT = 60~65，先验证稳定
2. 调 Kp，让偏线后能及时回来
3. 调 Kd，把蛇形震荡压下去
4. SPEED_STRAIGHT 提到 70、75、80...
5. 每个参数组合跑 2 轮，记录平均时间
6. 一旦开始明显压线/震荡，就退回上一档
```

不要一开始追求最高 PWM。两轮取平均时，“稍慢但稳定”的参数通常比“一轮很快、一轮跑飞”更划算。

## PWM

Timer0 软件 PWM：

```text
PWM 频率约 200 Hz
控制器更新频率约 200 Hz
PWM 分辨率约 4%
```

ENA / ENB 独立控制左右轮速度。

## 硬件接线

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

## 当前版本原则

不做：

- 自动中心识别
- 复杂环岛/路口状态机
- 多级速度规划
- 假的编码器速度 PID

只保留验收真正有用的：

- PD
- PWM
- 差速
- 一个简单的丢线按上一方向找回

这样更适合这次课程验收。
