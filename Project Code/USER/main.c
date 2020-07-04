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
*��finished the init of Model & System
*��finished the interface of Call Back Funcation
*��finished the package of Call Back Funcation
*��fix the bug:the address of reading over-current state of motor 
*��fix the bug:STOP_KEY control state isn't considered in Motor control
************************************************************************/

//����
extern u16 ADC_Conversion_Value[5];

/********************************************************************
�����ܣ�ϵͳ�ĳ�ʼ��
����汾��V1.0
��    �ڣ� 2019/5/30
��    �ߣ�Orange
��    �ģ���
********************************************************************/
void System_Init(void)
{
	//����ģ��ĳ�ʼ��
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
�����ܣ��ص������Ĵ���
����汾��V1.0
��    �ڣ� 2019/5/30
��    �ߣ�Orange
��    �ģ���
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
�����ܣ�Demo������
����汾��V1.0
��    �ڣ� 2019/5/30
��    �ߣ�Orange
��    �ģ���
********************************************************************/

int main(void)
{
	System_Init();
	
  	while(1)
	{
		/**************************************************
						ʱ��Ƭ��ѯ����
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
		WDG_Flg = 1;  //������λ
	}
	
	//return 0;
 }

