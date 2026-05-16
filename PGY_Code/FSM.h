#ifndef _FSM_H__
#define _FSM_H__

#include "UserInc.h"

extern float HALL_Comp[6];

extern float I_Drag_Frq;

extern float HallW_M[2];
extern float HallZ_M[2];
extern float HallX_M[2];
extern float HallY_M[2];

// defined by lx
// #define Standby 0  //待机状态
// #define ADCOfst 1  //上机预备状态
// #define PrePos  2  //预定位状态（没用到）
// #define Running 3  //运行状态
// #define ERROR   4  //错误状态
// #define AtoCalib 5 //自动机械位置偏差强拉矫正
// #define ZeroCurCalib 6 //手动机械位置零电流反电势矫正

// defined by pgy
#define AxisHallCalib 7 //输出端位置矫正
#define WriteToFlash 8 //将数据保存进入Flash
#define HallAdcCalib 9 //HALL采样校正

void FSM_SoftShut();

#endif