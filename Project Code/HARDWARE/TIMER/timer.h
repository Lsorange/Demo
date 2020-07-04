#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#define TIMER_PRSC 99
#define TIMER_PERD 7199
#define MOTOR_PRSC 899
#define MOTOR_PERD 0
#define STEER_PRSC 7199
#define STEER_PERD 199

#define Task1_Cnt 1
#define Task2_Cnt 2
#define Task3_Cnt 10
#define Task4_Cnt 100

void MTIMER_Init(void);
//void TskFlg_Init(void);
//void TIM1_Init(u16 arr, u16 psc);
//void Turn_PWM_Init(u16 arr, u16 psc);
//void Motor_PWM_Init(u16 arr,u16 psc);

typedef volatile struct
{
	u8 ADC_Current;
	u8 ADC_Voltage;
	u8 LED_Display;
	u8 KEY_SCAN;
	u8 MOTOR_Control;
	u8 STEER_Control;
	u8 STRInit_Flg;
	u8 USART_Control;
}Task_Flag;


#endif
