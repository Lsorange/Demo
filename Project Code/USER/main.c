#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "adc.h"
#include "usart.h"
#include "timer.h"
#include "steer.h"
#include "motor.h"
#include "wdg.h"
#include "Public_Value.h"

/************************************************************************
*Development Log:
*①finished the init of Model & System
*②finished the interface of Call Back Funcation
*③finished the package of Call Back Funcation
*④fix the bug:the address of reading over-current state of motor 
*④fix the bug:STOP_KEY control state isn't considered in Motor control
************************************************************************/

/********************************************************************
程序功能：系统的初始化
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
********************************************************************/
void System_Init(void)
{
	MLED_Init();
	MKEY_Init();
	MUSART_Init();
	MTIMER_Init();
	MCom_Init();
	MMotor_Init();
	MSTEER_Init();
	MADC_Init();
	MWDG_Init(); 
}

/********************************************************************
程序功能：回调函数的处理
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
********************************************************************/

void Handle_Func(void (*CallBack_Func)(void), u8 *Timer_Flg)
{
	if(NULL != CallBack_Func)
	{
		if(1 == (*Timer_Flg))
		{
			(*CallBack_Func)();
			(*Timer_Flg) = 0;
		}
		else
		{
			//do nothing
		}
	}
	else
	{
		return;
	}
} 

/********************************************************************
程序功能：Demo主程序
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
********************************************************************/

int main(void)
{
	System_Init();
	
  	while(1)
	{
		/**************************************************
				时间片轮询调度
		***************************************************/
		
		/************10ms task-433M control************/
		Handle_Func(MCom_CallBack, (u8*)&Tsk_Flg.USART_Control);
		
		/************20ms task-steer control*********/
		Handle_Func(MMotor_CallBack, (u8*)&Tsk_Flg.MOTOR_Control);
		
		/************20ms task-motor control*********/
		Handle_Func(MSTEER_CallBack, (u8*)&Tsk_Flg.STEER_Control);
		
		/************20ms task-LED control************/
		Handle_Func(MLED_CallBack, (u8*)&Tsk_Flg.LED_Display);
		
		/************10ms task-KEY control************/
		Handle_Func(MKEY_CallBack, (u8*)&Tsk_Flg.KEY_SCAN);
	
		/************100ms task-Current monitor****/
		Handle_Func(MCUR_CallBack, (u8*)&Tsk_Flg.ADC_Current);
		
		/************1000ms task-Voltage monitor****/
		Handle_Func(MVOL_CallBack, (u8*)&Tsk_Flg.ADC_Voltage);
		
		////////////////////////////////////////////////////
		
		/*****************task-WDG******************/
		Handle_Func(MWDG_CallBack, (u8*)&WDG_Flg); 
		WDG_Flg = 1;  //重新置位
	}
	
	//return 0;
 }

