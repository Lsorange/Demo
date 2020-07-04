#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "timer.h"
#include "Public_Value.h"


#define WORK_LED PBout(5)// PB5
#define DATA_LED PBout(6)// PB6
#define RETURN_LED PBout(7)// PB7
#define FORWARD_LED PBout(8)// PB8

#define JOG_LED1 PCout(11)// PC11
#define JOG_LED2 PCout(12)// PC12
#define MOTOR_OFF  0x01
#define MOTOR_ON   0x02 

//≤‚ ‘”√
#define KEY_Sts_LED PAout(5)

#if TEST_START
	#define KEY_Test_LED  PEout(5)
	#define KEY_Two_LED   PEout(6)
	#define KEY_Three_LED PEout(2)
	void LED_Test(void);
#endif


void MLED_CallBack(void);
void MLED_Init(void);
void DataConnect_Sts(void);
//void RLEDSts_Init(void);
//void LED_Init(void);
//void Return_LED(void);
//void KeyStp_LED(void);
//void KeyFrnt_LED(void);
//void KEY_To_LED(void);

		 				    
#endif
