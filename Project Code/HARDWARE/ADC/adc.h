#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

#define FILT_CNT             10
#define Over_Current         2
//#define Vref_Voltage       3.3
#define Vref_Value           4095
#define Full_Battery         3
#define Bat_FulVol  	     3273    // ((Full_Battery) * (Vref_Value)) / (Vref_Voltage)
#define Bat_HigVol           2978    // ((Full_Battery) * (Vref_Value) * 0.8) / (Vref_Voltage)
#define Bat_MidVol           1636    // ((Full_Battery) * (Vref_Value) * 0.5) / (Vref_Voltage)
#define Bat_LowVol           982     // ((Full_Battery) * (Vref_Value) * 0.3) / (Vref_Voltage)
#define Over_Current_Volatge 2482    // ((Over_Current) * (Vref_Value)) / (Vref_Voltage)

void MCUR_CallBack(void);
void MVOL_CallBack(void);
void MADC_Init(void);
//void ADC1_GPIO_Config(void);
//void ADC1_Config(void);
//void ADC1_DMA_Config(void);
//void ADCFlg_Init(void);
//void ADC_Convert(void);
//void Cur_Montr(void);
//void Vol_Montr(void);

typedef volatile struct
{
	u8 M1_OVERCUR : 1;
	u8 M2_OVERCUR : 1;
	u8 M3_OVERCUR : 1;
	u8 M4_OVERCUR : 1;
	u8 BAT_LOWVOL : 1;
	u8 BAT_MIDVOL : 1;
	u8 BAT_HIGVOL : 1;
	u8 BAT_FULVOL : 1;
}ADC_FLAG;

#endif

