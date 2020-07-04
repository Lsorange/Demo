#ifndef __Public_Value_H
#define __Public_Value_H
#include "sys.h"
#include "timer.h"
#include "adc.h"
#include "433m.h"
#include "usart.h"
#include "wdg.h"
#include "key.h"

#define TEST_START  1
#define TEST_END    0

/*******主&扩展电机方向信号*******/
extern u8 RMainMotor_State;
extern u8 RExtdMotor_State;
extern u8 RMainMotor_Spd;
extern u8 RExtdMotor_Spd;

/*******LED指示灯的信号**********/
extern u8 RLED_State;

/******电机停止按键扫描的函数***********/
extern u8 KEY_Scan(void);
//extern sKeySts KeySts_gu8;

/******定时器轮询的标志位*********/
extern Task_Flag Tsk_Flg;

/****过流、蓄电池电压信号,电流采集DMA转换的结果****/
extern ADC_FLAG ADCMont_Flg;
extern u16 ADC_Conversion_Value[5];
extern u8 *pADCFlg;
//extern u8 Flag_DMA;

/*********舵机控制信号************/
//extern u8 RSteer1_Angle;
//extern u8 RSteer2_Angle;  //1-20

/*********数据连接状态************/
//extern const u8 DataCont_Sts;

/*********串口数据接收***********/
extern RecvData_Struct Recv_Data;
extern volatile u8 RX_Flg;
extern volatile u8 Data_Len;
extern volatile u8 State_SW;

/*********看门狗的状态***********/
extern volatile u8 WDG_Flg;

#endif

