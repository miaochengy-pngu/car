# STC89C52RC 红外循迹竞赛小车

这是一个完整的 Keil C51 工程。当前版本已经从最初的四状态 demo 控制升级为：

- Timer0 软件 PWM
- L298N ENA / ENB 独立调速
- 整数定点 PID 控制器
- 默认 PD 转向控制（完整 I 项已实现，初始 Ki=0）
- 直线高速 / 弯道自动降速
- 短时转向记忆
- 丢线 / 特殊状态按上一转向方向搜索
- 启动时自动判断“居中状态是 00 还是 11”
- 1 kHz 控制循环
- 200 Hz 电机 PWM

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
├─ System/
│  ├─ config.h
│  ├─ delay.c
│  └─ delay.h
├─ docs/
│  ├─ HARDWARE_NOTES.md
│  └─ OPEN_SOURCE_REVIEW.md
└─ build.bat
```

双击：

```text
Project/car.uvproj
```

即可打开 Keil C51 工程。

## 当前比赛控制链

```text
OTL / OTR
    ↓
Tracking
    ↓
离散位置误差 + 转向记忆
    ↓
PID / PD
    ↓
自适应基础速度 + 差速修正
    ↓
signed left/right motor command
    ↓
200 Hz software PWM
    ↓
ENA / ENB
    ↓
L298N
```

## 关键调参位置

所有首要参数集中在：

```text
System/config.h
```

当前初值：

```c
#define STEER_KP_X100          24
#define STEER_KI_X100           0
#define STEER_KD_X100          18

#define SPEED_STRAIGHT          88
#define SPEED_MIN               42
#define SPEED_MAX               96
#define SPEED_SLOWDOWN_MAX      34

#define SEARCH_FORWARD_SPEED    62
#define SEARCH_REVERSE_SPEED    26
#define STEERING_MEMORY_TICKS   12
```

### 推荐调参顺序

1. 先确认左右电机方向、红外极性完全正确。
2. 先保持 `Ki=0`。
3. 从较低 `SPEED_STRAIGHT` 开始确认不丢线。
4. 提高 `Kp` 直到能迅速回线，但不要蛇形振荡。
5. 提高 `Kd` 抑制振荡和冲过头。
6. 再逐步提高 `SPEED_STRAIGHT`。
7. 高速弯道容易飞线时，提高 `SPEED_SLOWDOWN_MAX` 或降低 `SPEED_MIN`。
8. 只有出现长期固定偏差时，再尝试很小的 `Ki`。

## 硬件接线

### L298N 控制

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

### 红外循迹

```text
OTL -> P3.5
OTR -> P3.4
GND -> GND
VCC -> VCC
```

模块逻辑：

```text
白底 -> 0 -> 指示灯亮
黑线 -> 1 -> 指示灯灭
```

## 启动方式

因为双数字传感器可能有两种安装几何，程序不再强行假设居中一定是 `00`。

比赛启动时：

1. 把车摆正、居中放在黑线上；
2. 再打开主控板电源；
3. 程序等待约 0.5 s；
4. 自动采样约 0.13 s；
5. 自动确定居中是 `00` 还是 `11`；
6. 随后进入闭环循迹。

## 关于“速度 PID”

本套件原配没有轮速编码器，因此不能凭空实现真实的左右轮速度闭环 PID。

当前实现的是：

- **方向 PID/PD**：根据红外误差计算左右差速；
- **PWM 速度调节**：通过 ENA/ENB 改变左右电机占空比；
- **自适应速度规划**：直道快、转弯自动降速。

如果以后加编码器，再增加左右轮独立速度 PI/PID，形成“方向外环 + 轮速内环”。

## 开源调研

见：

```text
docs/OPEN_SOURCE_REVIEW.md
```

其中记录了本版本参考的开源 line-follower 项目及实际采用的控制策略。
