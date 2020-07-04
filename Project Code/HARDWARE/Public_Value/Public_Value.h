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

/*******��&��չ��������ź�*******/
extern u8 RMainMotor_State;
extern u8 RExtdMotor_State;
extern u8 RMainMotor_Spd;
extern u8 RExtdMotor_Spd;

/*******LEDָʾ�Ƶ��ź�**********/
extern u8 RLED_State;

/******���ֹͣ����ɨ��ĺ���***********/
extern u8 KEY_Scan(void);
//extern sKeySts KeySts_gu8;

/******��ʱ����ѯ�ı�־λ*********/
extern Task_Flag Tsk_Flg;

/****���������ص�ѹ�ź�,�����ɼ�DMAת���Ľ��****/
extern ADC_FLAG ADCMont_Flg;
extern u16 ADC_Conversion_Value[5];
extern u8 *pADCFlg;
//extern u8 Flag_DMA;

/*********��������ź�************/
//extern u8 RSteer1_Angle;
//extern u8 RSteer2_Angle;  //1-20

/*********��������״̬************/
//extern const u8 DataCont_Sts;

/*********�������ݽ���***********/
extern RecvData_Struct Recv_Data;
extern volatile u8 RX_Flg;
extern volatile u8 Data_Len;
extern volatile u8 State_SW;

/*********���Ź���״̬***********/
extern volatile u8 WDG_Flg;

#endif

