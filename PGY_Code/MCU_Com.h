#ifndef _MCU_COM_H__
#define _MCU_COM_H__

#include "UserInc.h"

typedef enum {
    Type_torque_SPI,   // 力矩模式
    Type_velocity_SPI, // 速度模式
    Type_angle_SPI,    // 位置模式
    Type_FPVc_SPI,     // 力位混合模式
} MotionControlType_SPI;

typedef enum {
    PosZero_SPI,
    RigStandby_SPI,
    RelxStandby_SPI,
    Running_SPI,
} MotorStatusType_SPI;

typedef struct
{
    uint8_t motor_status_SPI;  // 这些数据是MCU2传给MCU1
    uint8_t Motor_Control_SPI; // 这些数据是MCU2传给MCU1
    uint16_t FPV_KP_SPI;       // 这些数据是MCU2传给MCU1
    uint16_t FPV_KD_SPI;       // 这些数据是MCU2传给MCU1
    float Pos_ref_SPI;      // 这些数据是MCU2传给MCU1
    float Vel_ref_SPI;      // 这些数据是MCU2传给MCU1
    float Torq_ref_SPI;     // 这些数据是MCU2传给MCU1
} TPV_DATA_T_2TO1_SPI;

typedef struct
{
    float Pos_actual_SPI;          // 这些数据是MCU1传给MCU2
    float Vel_actual_SPI;          // 这些数据是MCU1传给MCU2
    float Torq_actual_SPI;         // 这些数据是MCU1传给MCU2
    uint16_t motor_status_actual_SPI; // 这些数据是MCU1传给MCU2
    uint16_t motor_temp_SPI;          // 这些数据是MCU1传给MCU2
} TPV_DATA_T_1TO2_SPI;
typedef struct
{
    TPV_DATA_T_2TO1_SPI TPV_data_2to1_SPI; // 这些数据是MCU2传给MCU1

    TPV_DATA_T_1TO2_SPI TPV_data_1to2_SPI; // 这些数据是MCU1传给MCU2
} TPV_DATA_T_SPI;

extern TPV_DATA_T_SPI TPV_DATA_SPI;
extern MotionControlType_SPI Motor_Control_SPI;
extern MotorStatusType_SPI Motor_status_SPI;
extern TPV_DATA_T_1TO2_SPI TPV_data_2to1_SPI;
extern TPV_DATA_T_1TO2_SPI TPV_data_1to2_SPI;

void User_Communication_Init(void); // MCU1的SPI初始化配置
void User_Communication_Loop(void); // MCU1的循环执行发送和接收函数

void User_Communication_Init2(void); // MCU2的SPI初始化配置
void User_Communication_Loop2(void); // MCU2的循环执行发送和接收函数

#endif
