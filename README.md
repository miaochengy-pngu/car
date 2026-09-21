# STC89C52RC 红外循迹小车

这是一个**完整的 Keil C51 工程**，不是单个 `main.c`。

目录结构：

```text
car/
├─ Project/
│  └─ car.uvproj          # Keil 工程，直接双击打开
├─ User/
│  └─ main.c              # 主程序
├─ Hardware/
│  ├─ Motor/
│  │  ├─ motor.c
│  │  └─ motor.h
│  └─ Tracking/
│     ├─ tracking.c
│     └─ tracking.h
├─ System/
│  ├─ config.h
│  ├─ delay.c
│  └─ delay.h
├─ docs/
│  └─ HARDWARE_NOTES.md
├─ build.bat
└─ README.md
```

> 8051/C51 不像 STM32 HAL 那样需要把一大套官方 HAL 源文件复制进工程。像 `REG52.H`、C51 运行库等由 Keil C51 工具链自带；仓库里放的是我们自己的板级驱动和应用模块。

## 直接打开

Windows 下安装 Keil C51 后：

1. 双击 `Project/car.uvproj`。
2. 工程树会显示 `USER`、`HARDWARE`、`SYSTEM` 三组。
3. 点击 Build。
4. 工程已开启 **Create HEX File**，HEX 会生成到 `Project/Objects/`。

也可以在仓库根目录双击 `build.bat` 编译；默认 Keil 路径为：

```text
C:\Keil_v5\UV4\UV4.exe
```

## 硬件

- STC89C52RC-DIP40
- 11.0592 MHz
- L298N
- 双路 TCRT5000 + LM393
- 2 × TT 减速电机
- 4 × 1.5 V AA 电池

## 电机控制引脚

说明书与实物 PCB 一致：

| L298N | C51 主控板 | MCU |
|---|---|---|
| ENB | P10 | P1.0 |
| IN4 | P11 | P1.1 |
| IN3 | P12 | P1.2 |
| IN2 | P13 | P1.3 |
| IN1 | P14 | P1.4 |
| ENA | P15 | P1.5 |

电机线：

```text
左电机红线 -> OUT2
左电机黑线 -> OUT1

右电机红线 -> OUT4
右电机黑线 -> OUT3
```

## 红外循迹接口

```text
OTL -> P3.5
OTR -> P3.4
GND -> GND
VCC -> VCC
```

模块逻辑：

```text
白底 -> 输出 0 -> 指示灯亮
黑线 -> 输出 1 -> 指示灯灭
```

程序当前策略：

| 左 OTL | 右 OTR | 动作 |
|---:|---:|---|
| 0 | 0 | 前进 |
| 1 | 0 | 左修正 |
| 0 | 1 | 右修正 |
| 1 | 1 | 停车 |

## 各文件职责

- `User/main.c`：上电初始化与主循环。
- `Hardware/Motor/motor.c`：L298N、左右电机和整车动作。
- `Hardware/Tracking/tracking.c`：两路红外读取与循迹状态机。
- `System/delay.c`：11.0592 MHz 下的基础延时。
- `System/config.h`：循迹逻辑、转向方式、控制周期等配置。

后续需要加 PWM 时，会单独增加 PWM 驱动模块，而不是继续把所有代码塞进 `main.c`。
