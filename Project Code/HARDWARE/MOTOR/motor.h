#ifndef  __MOTOR_H
#define  __MOTOR_H
#include "sys.h"

#define IN_1A    PBout(12)// PB12
#define IN_1B    PBout(13)// PB13

#define IN_2A    PBout(14)// PB14
#define IN_2B    PBout(15)// PB15

#define IN_3A    PCout(6)// PC6
#define IN_3B    PCout(7)// PC7

#define IN_4A    PCout(8)// PC8
#define IN_4B    PCout(9)// PC9

//暂时给个最大的有效值，该值对应arr值，需要注意！！！

#define   Speed_Max    899
#define   MDelay_Time  500    //大概4-5s的样子
#define   Basic_SpdVal 150
#define   Main_Type    1
#define   Extd_Type    0
#define   MOTOR_ST     2   //电机启动


//u8 Read_MotorNum(void);
//u8 Read_MotorMode(u8 Motor_State);
//u8 Read_MotorSpd(u8 Motor_Spd);
//extern u8 Read_MotorFlg(u8 Motor_Flg);
//u8 Read_MotorCurSts(u8 Cur_Sts, u8 Motor_Type);

void MMotor_CallBack(void);
void MMotor_Init(void);
//void MotorDir_Init(void);
//void Motor_Control(void);
//void MainMotor_Control(u16 time, u8 mode, u8 speed_mode);
//void ExtdMotor_Control(u16 time, u8 mode, u8 speed_val);
//void MainMotor_Init(void);
//void ExtdMotor_Init(void);
//void MainMotor_SpdMode(u8 spd_val);
//void ExtdMain_SpdMode(u8 spd_level);
//void MainMotor_Forward(void);
//void ExtdMotor_Forward(void);
//void MainMotor_Reverse(void);
//void ExtdMotor_Reverse(void);

#endif
