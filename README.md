# STC89C52RC 红外循迹小车

面向课程套件的两轮红外循迹小车程序。当前硬件按实物照片与公开资料整理：

- 主控：STC89C52RC / 8051 兼容，Keil C51
- 循迹：双路 TCRT5000 + LM393 数字比较器板，接口标注 `OTL / OTR / GND / VCC`
- 驱动：L298N 双 H 桥，实际红色驱动板控制端为 `IN1 / IN2 / IN3 / IN4`
- 执行器：2 × TT 直流减速电机

## 当前默认接线

为了避免依赖主控板上未知的“专用白色接口”内部走线，第一版直接使用板上已经引出的 MCU IO 排针。这样最容易核对，也最容易排错。

| 模块 | 信号 | STC89C52RC |
|---|---|---|
| 双路循迹 | OTL（左） | P3.3 |
| 双路循迹 | OTR（右） | P3.2 |
| 双路循迹 | VCC | +5V |
| 双路循迹 | GND | GND |
| L298N | IN1 | P1.2 |
| L298N | IN2 | P1.3 |
| L298N | IN3 | P1.6 |
| L298N | IN4 | P1.7 |
| L298N | GND | 主控板 GND（必须共地） |
| L298N | MOTOR A / OUT1-OUT2 | 左电机 |
| L298N | MOTOR B / OUT3-OUT4 | 右电机 |

> 如果实车“前进”时某一侧轮子反转，最简单的处理是交换该侧电机接在 L298N 输出端的两根线，不必改程序。

## 传感器逻辑

这类双路循迹板使用 TCRT5000 反射式红外器件和比较器。公开资料中与实物板型一致的模块标注为：

- 白色表面反射强：数字输出低电平
- 黑线反射弱：数字输出高电平

因此默认程序使用：

| OTL | OTR | 判断 | 动作 |
|---:|---:|---|---|
| 0 | 0 | 两侧均为白底，黑线位于中间 | 前进 |
| 1 | 0 | 左侧压到黑线 | 左修正 |
| 0 | 1 | 右侧压到黑线 | 右修正 |
| 1 | 1 | 两侧同时检测黑色 | 停车（可作为终点线） |

如果你手里的模块实测逻辑相反，只需要把 `main.c` 顶部的 `TRACK_BLACK_LEVEL` 从 `1` 改成 `0`。

## 转弯方式

默认使用“单侧停、另一侧前进”的弧线修正，比较适合第一次调车：

- 左修正：左轮停，右轮前进
- 右修正：右轮停，左轮前进

如果赛道弯很急，可把 `TURN_STYLE` 改为 `TURN_STYLE_SPIN`，程序会采用一侧前进、一侧反转的原地差速修正。

## Keil C51 编译

1. 新建 Keil uVision 工程。
2. Device 可选兼容 8051/AT89C52 的目标；STC89C52RC 使用标准 8051 SFR，本程序包含 `REG52.H`。
3. 将仓库根目录的 `main.c` 加入 Source Group。
4. Options for Target -> Output -> 勾选 **Create HEX File**。
5. Build。
6. 用 STC-ISP 选择你的实际 STC89C52RC 型号并下载生成的 HEX。

## 第一次上车前

先把车轮架空测试：

1. 上电后先确认两个电机都不会乱转。
2. 用白纸放在两个传感器下面，应前进。
3. 只让左探头对准黑线，应左修正。
4. 只让右探头对准黑线，应右修正。
5. 两个探头同时压黑线，应停车。

若左右修正方向刚好反了，优先交换 `OTL` 与 `OTR` 两根信号线。

## 供电注意

你的实物电池盒看起来是 4 节电池。电机电源和 MCU 5V 逻辑电源不要凭模块名字直接并接：

- L298N 的电机电源接电池；
- MCU 和循迹模块应得到稳定的 5V；
- MCU、循迹模块、L298N **GND 必须共地**；
- 若 L298N 板载 5V 稳压器需要较高输入压差，而电池只有约 6V，不要默认它的 5V 输出仍然稳定。

## 公开参考资料

代码不是直接复制博客，而是依据器件逻辑、公开原理图和开源实现重新整理。

- STC 官方 STC89C52RC：<https://www.stcmicro.com/STC/STC89C52RC.html>
- Vishay TCRT5000：<https://www.vishay.com/en/product/83760/>
- ST L298：<https://www.st.com/en/motor-drivers/l298.html>
- 嘉立创 EDA STC89C52 智能小车开源课程：<https://wiki.lceda.cn/zh-hans/course-projects/mcu51/89c52-smartcar/introduce.html>
- 电子科技大学《循迹避障智能小车的实验设计》（含电机/循迹/供电原理图）：<https://www.sy.uestc.edu.cn/article/doi/10.12179/1672-4550.20200009?viewType=HTML>
- GitHub：Teeerry/51-smartcar（STC89C52RC + 红外循迹 + 电机控制）：<https://github.com/Teeerry/51-smartcar>
- 与实物双路循迹板外形/标注一致的说明图：<https://blog.csdn.net/ShuaibeiJia/article/details/135315797>

后续如果确认老师要求 PWM 调速、特殊路口、黑线宽度或赛道形状，再在此版本上增加状态机/PWM，不需要推倒重写。
