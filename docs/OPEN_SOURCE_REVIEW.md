# Open-source competition controller review

This note records the control ideas reviewed before upgrading the A11 firmware.

## Repositories reviewed

### beastbroak30/Microcontroller-prj

File:
- `PID_Linefollower/PID_Linefollowerv2.ino`

Relevant ideas:
- L298N ENA/ENB PWM speed control.
- PID steering correction.
- Separate straight/base speed and aggressive turn/search speed.
- Integral anti-windup.
- Explicit line-loss recovery using the previous error direction.

URL:
https://github.com/beastbroak30/Microcontroller-prj

### GusGus196/carro_iot

File:
- `lib/line_follower/LineFollower.cpp`

Relevant ideas:
- PD is often preferable to a strong I term for line following.
- Derivative damping reduces overshoot.
- Speed is reduced as steering error grows.
- A decaying steering memory helps bridge short line gaps / sensor transitions.

URL:
https://github.com/GusGus196/carro_iot

### jckuri/line-follower-robot

Relevant ideas:
- L298N ENA/ENB are intended for PWM speed regulation.
- Sensor position should be mapped into a steering error and motor speed difference.
- Last-seen line direction is useful when the line is no longer visible.

URL:
https://github.com/jckuri/line-follower-robot

### felipemmattia/PID_line_follower_robot

Relevant ideas:
- Competition-oriented adaptive speed control.
- Raise straight-line speed while slowing automatically on large errors / tight curves.
- PID tuning is coupled to base speed.

URL:
https://github.com/felipemmattia/PID_line_follower_robot

## What was adapted to this STC89C52RC car

The reviewed projects usually use 5-14 sensors or analog sensor arrays. This A11 kit currently has only two digital TCRT5000 outputs, so copying their weighted-position math would be invalid.

The current firmware therefore adapts the ideas that are actually supported by this hardware:

1. Timer0 software PWM drives L298N ENA/ENB independently.
2. Steering uses a full PID module, initially tuned as PD (`Ki=0`).
3. Motor commands are signed and independently adjustable from -100 to +100.
4. Straight-line PWM is high; base speed is reduced as steering correction increases.
5. Previous steering direction is remembered for short sensor gaps and ambiguous states.
6. Startup auto-detects whether the centered sensor pattern is `00` or `11`.
7. A 1 kHz control update is separated from the 200 Hz motor PWM.

## Hardware limitation

Two digital sensors only provide a very small number of observable line states. They cannot produce the same smooth continuous position estimate as a 5/8-channel analog array.

Therefore:

- adding PID/PWM can materially improve stability and speed over bang-bang control;
- but the sensor module remains the limiting factor at high speed;
- a true wheel-speed PID additionally requires wheel encoders, which this kit does not currently have.

The firmware does not pretend that an encoder speed loop exists. It implements steering PID/PD plus open-loop PWM speed scheduling, which is the strongest control stack available from the current hardware without adding sensors/encoders.
