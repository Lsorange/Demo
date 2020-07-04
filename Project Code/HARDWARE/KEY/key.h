#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define N_KEY            0        //无按键按下
#define S_KEY            1        //有按键按下且为短按
#define L_KEY            2        //有按键按下且为长按

#define N_KEY_State      0        //无按键
#define S_KEY_State      1        //短按，电机停止
#define L_KEY_State      2        //长按，电机重新启动
#define KDelay_Time      200    //3-4s，该数得以实际的情况确定

//#define Motor_Stop_KEY  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)

//测试用
#define Motor_Stop_KEY  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)


void MKEY_CallBack(void);
void MKEY_Init(void);
//void Stop_KEY_Init(void);
//u8 Get_KEY_State(u16 time);
u8 KEY_Scan(void);

		
#endif
