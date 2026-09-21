# 硬件资料核对记录

本文件记录为什么当前代码采用这些默认值，后续如果老师给出正式原理图，可以直接逐项替换。

## 1. 主控

实物照片中的 40 Pin DIP 芯片为 STC 8051 系列板，外形和丝印与 STC89C52RC 教学板一致；程序按 STC89C52RC/标准 8051 端口编写。

STC 官方资料：

- <https://www.stcmicro.com/STC/STC89C52RC.html>
- STC89C52RC：8 KB Flash、512 B RAM、P0/P1/P2/P3 GPIO、ISP/IAP。
- P3.2/P3.3 同时也是 INT0/INT1，但在本项目不启用外部中断，因此可直接作普通数字输入。

公开课程原理图可交叉参考：

- 嘉立创 EDA STC89C52 智能小车：
  <https://wiki.lceda.cn/zh-hans/course-projects/mcu51/89c52-smartcar/introduce.html>
- 电路设计页：
  <https://wiki.lceda.cn/zh-hans/course-projects/microcontroller/89c52-smartcar/circuit-design.html>

当前不假设照片中主控板白色 JST 插座的具体内部网络，而是直接使用板上标明的 P1.x / P3.x IO 排针，避免拿错不同厂家的接口定义。

## 2. 双路循迹板

实物板型与公开的双路循迹模块高度一致，核心特征：

- 左右两路红外反射探头；
- 两个灵敏度电位器；
- 比较器；
- 四线输出 `OTL / OTR / GND / VCC`；
- 左右指示灯。

与实物板型一致的公开说明：

<https://blog.csdn.net/ShuaibeiJia/article/details/135315797>

该类模块的数字逻辑：

- 白色：红外反射强 -> 比较器数字输出通常为 0；
- 黑色：红外反射弱 -> 数字输出通常为 1。

Vishay TCRT5000 官方资料：

- <https://www.vishay.com/en/product/83760/>
- <https://www.vishay.com/docs/83760/tcrt5000.pdf>

注意：TCRT5000 本体只是红外 LED + 光电晶体管；最终高低电平还取决于模块上的比较器接法，因此代码保留 `TRACK_BLACK_LEVEL` 开关，实车一分钟即可确认。

## 3. L298N 电机驱动

课堂 PPT 与实物均为 L298N 双 H 桥体系。实物红色板上可直接看到 `IN1/IN2/IN3/IN4` 控制输入和两组电机输出。

ST 官方 L298：

<https://www.st.com/en/motor-drivers/l298.html>

L298 的核心关系：

- IN1/IN2 控制桥 A；
- IN3/IN4 控制桥 B；
- 每组输入一高一低时电机正/反转；
- 两输入都低时可停止。

常见红色 L298N 模块接线资料：

<https://wiki.lckfb.com/zh-hans/coloreasyduino/module/control/l298n-motor-drive-module.html>

当前实物驱动板照片没有清晰看到独立引出的 ENA/ENB PWM 控制，因此第一版固件只控制 IN1-IN4，不强行假设 PWM 使能脚可用。完成基本循迹后，如果确认板上 ENA/ENB 的实际接口，再加 Timer0 软件 PWM。

## 4. 开源代码交叉验证

GitHub 开源项目：

<https://github.com/Teeerry/51-smartcar>

它同样采用 STC89C52RC，并使用：

- P3.2/P3.3 读取两路红外；
- P1.2/P1.3、P1.6/P1.7 控制两侧电机；
- 左右电机因机械安装方向相反而采用相反的前进电平组合。

当前仓库采用同一组易于验证的 IO 分配，但循迹代码为重新整理后的独立实现。

## 5. 学术/教学原理图参考

电子科技大学实验教学论文《循迹避障智能小车的实验设计》直接给出了：

- 电机模块原理图；
- 循迹模块原理图；
- 供电模块原理图；
- 循迹控制流程图。

链接：

<https://www.sy.uestc.edu.cn/article/doi/10.12179/1672-4550.20200009?viewType=HTML>

另一篇 STC89C52 + L298N + TCRT5000 系统方案：

<https://www.21ic.com/app/mcu/201402/206047.htm>

## 6. 当前最重要的实车核对项

在进一步写 PWM / PID / 路口状态机之前，只需确认四件事：

1. 芯片完整丝印是否确为 STC89C52RC；
2. 白纸与黑胶带下，OTL/OTR 的高低电平是否与默认一致；
3. L298N 的 IN1-IN4 插座顺序是否与 PCB 丝印一致；
4. 左/右电机接在 MOTOR A/B 后，程序的 `car_forward()` 是否真的让整车向前。

这四项确认后，后续代码基本只涉及控制策略，不再涉及硬件猜测。
