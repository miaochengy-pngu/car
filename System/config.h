#ifndef __CAR_CONFIG_H__
#define __CAR_CONFIG_H__

/*
 * 下面只保留开源例程真正需要的三个动作参数。
 *
 * 开源例程原始做法：
 *   00 -> run()
 *   10 -> leftrun()
 *   01 -> rightrun()
 *   11 -> 不执行新动作，保持上一条电机命令
 *
 * 原例程的占空比较低；你的实车低占空比起不来，
 * 所以只把占空比按实车提高，控制逻辑不改。
 */
#define RUN_LEFT_SPEED          90
#define RUN_RIGHT_SPEED         90

#define TURN_INNER_SPEED      -100
#define TURN_OUTER_SPEED       100

#endif
