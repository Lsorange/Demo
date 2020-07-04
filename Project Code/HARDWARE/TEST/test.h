#ifndef __TEST_H
#define __TEST_H

#include "Public_Value.h"
#include "led.h"
#include "433m.h"

/***************LED test case************/

#if defined (T_LED_Start)

	u8 TLED_State = 0x00;                  //0x1f所有的led均on
	Recv_Data.RLED_State = TLED_State;
	
	//KEY_Test_LED = 1;

#undef T_LED_Start
#endif

#if defined (T_LED_End)
#undef T_LED_End
#endif
/////////////////////////////////////////

/***************KEY test case***********/

#if defined (T_KEY_Start)

//test.case

#undef T_KEY_Start
#endif

#if defined (T_KEY_End)
#undef T_KEY_End
#endif
/////////////////////////////////////////

/***************433M test case***********/

#if defined (T_COM_Start)

//test.case

#undef T_COM_Start
#endif

#if defined (T_COM_End)
#undef T_COM_End
#endif
/////////////////////////////////////////

/***************MOTOR test case***********/

#if defined (T_MOTOR_Start)

	#if defined (TEST_MOTOR)
		Recv_Data.RMainMotor_State = (0x01 + 0x20);
	#else
		Recv_Data.RMainMotor_State = (0x01 + 0x04);
	#endif
	
	Recv_Data.RExtdMotor_State = 0x06;
	Recv_Data.RMainMotor_Spd = 0;
	Recv_Data.RExtdMotor_Spd = 0;

#undef T_MOTOR_Start
#endif

#if defined (T_MOTOR_End)
#undef T_MOTOR_End
#endif
/////////////////////////////////////////

/***************STEER test case***********/

#if defined (T_STEER_Start)

//test.case

#undef T_STEER_Start
#endif

#if defined (T_STEER_End)
#undef T_STEER_End
#endif
/////////////////////////////////////////

/***************ADC test case***********/

#if defined (T_ADC_Start)

//test.case

#undef T_ADC_Start
#endif

#if defined (T_ADC_End)
#undef T_ADC_End
#endif
/////////////////////////////////////////

/***************WDG test case***********/

#if defined (T_WDG_Start)

//test.case

#undef T_WDG_Start
#endif

#if defined (T_WDG_End)
#undef T_WDG_End
#endif
/////////////////////////////////////////

#endif

