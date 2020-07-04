#include "adc.h"
#include "sys.h"
#include "delay.h"
#include "Public_Value.h"

/***********************************************************************
ȫ�ֱ�����bit����
------------------------------------------------------------------------
Name								bit					Defination
------------------------------------------------------------------------
Task_Flag	  						7					BAT_FULVOL
									6					BAT_HIGVOL
									5					BAT_MIDVOL
									4					BAT_LOWVOL
									3					M4_OVERCUR
									2					M3_OVERCUR
									1					M2_OVERCUR
									0					M1_OVERCUR
------------------------------------------------------------------------
************************************************************************/
ADC_FLAG ADCMont_Flg;
u8 *pADCFlg = (u8*)&ADCMont_Flg;
u16 ADC_Conversion_Value[5];       //ADC�ɼ�ֵ�洢������

static void ADCFlg_Init(void);
static void ADC1_GPIO_Config(void);
static void ADC1_Config(void);
static void ADC1_DMA_Config(void);
static void ADC_Convert(void);
static void Cur_Montr(void);
static void Vol_Montr(void);


/*******************************************************************
�����ܣ������ɼ��Ļص�������100ms task
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

void MCUR_CallBack(void)
{
	ADC_Convert();
	Cur_Montr();
}

/*******************************************************************
�����ܣ���ѹ�ɼ��Ļص�������1000ms task
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

void MVOL_CallBack(void)
{
	ADC_Convert();
	Vol_Montr();
}

/*******************************************************************
�����ܣ�ADCģ��ĳ�ʼ��
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

void MADC_Init(void)
{
	ADCFlg_Init();
	ADC1_GPIO_Config();
	ADC1_Config();
	ADC1_DMA_Config();
}

/*******************************************************************
�����ܣ���ѹ�����ĳ�ʼ��
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void ADCFlg_Init(void)
{
	u8 *pAdc = (u8*)&ADCMont_Flg;
	u8 i;

	for(i = 0; i < sizeof(ADC_FLAG)/sizeof(u8); i++)
	{
		pAdc[i] = 0x00;
	} 
}

/*******************************************************************
�����ܣ�ADC1��GPIO����
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		               
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
 
/*******************************************************************
�����ܣ�ADC1����
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void ADC1_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  //ʹ��ADC1ͨ��ʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	ADC_DeInit(ADC1);  //��λADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	      //ģ��ת��������ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 5;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ��� 

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_239Cycles5);
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);	    //ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
	
}

/*******************************************************************
�����ܣ�ADC1_DMA����
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void ADC1_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	
	DMA_DeInit(DMA1_Channel1);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ

	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADC1 -> DR));  //DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(ADC_Conversion_Value[0]);  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴����赽�ڴ�
	DMA_InitStructure.DMA_BufferSize = 5;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;      //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

/*******************************************************************
�����ܣ�ADC�ɼ�ֵת��
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void ADC_Convert(void)
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

/*******************************************************************
�����ܣ���������ļ��
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void Cur_Montr(void)
{
	static u8 M1_Cnt, M2_Cnt, M3_Cnt, M4_Cnt;
	
	//���M1����ĵ�ǰ����״̬
	if(ADC_Conversion_Value[0] > Over_Current_Volatge)
	{
		M1_Cnt++;
		if(M1_Cnt >= FILT_CNT)  //�˲�����
		{
			ADCMont_Flg.M1_OVERCUR = 0x01;
			M1_Cnt = 0;
		}
		else
		{
			//do nothing;
		}
	}
	else 
	{
		M1_Cnt = 0;
		ADCMont_Flg.M1_OVERCUR = 0;
	}
	
	//���M2����ĵ�ǰ����״̬
	if(ADC_Conversion_Value[1] > Over_Current_Volatge)
	{
		M2_Cnt++;
		if(M2_Cnt >= FILT_CNT)
		{
			ADCMont_Flg.M2_OVERCUR = 0x01;
			M2_Cnt = 0;
		}
		else
		{
			//do nothing;
		}
	}
	else 
	{
		M2_Cnt = 0;
		ADCMont_Flg.M2_OVERCUR = 0;
	}
	
	//���M3����ĵ�ǰ����״̬
	if(ADC_Conversion_Value[2] > Over_Current_Volatge)
	{
		M3_Cnt++;
		if(M3_Cnt >= FILT_CNT)
		{
			ADCMont_Flg.M3_OVERCUR = 0x01;
			M3_Cnt = 0;
		}
		else
		{
			//do nothing;
		}
	}
	else 
	{
		M3_Cnt = 0;
		ADCMont_Flg.M3_OVERCUR = 0;
	}
	
	//���M4����ĵ�ǰ����״̬
	if(ADC_Conversion_Value[3] > Over_Current_Volatge)
	{
		M4_Cnt++;
		if(M4_Cnt >= FILT_CNT)
		{
			ADCMont_Flg.M4_OVERCUR = 0x01;
			M4_Cnt = 0;
		}
		else
		{
			//do nothing;
		}
	}
	else 
	{
		M4_Cnt = 0;
		ADCMont_Flg.M4_OVERCUR = 0;
	}
}

/*******************************************************************
�����ܣ����ص�ѹ�ļ��
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void Vol_Montr(void)
{
	/****������ص�ѹ��״̬****/
	if(ADC_Conversion_Value[4] >= Bat_FulVol)
	{
		*pADCFlg = (*pADCFlg) & 0x0f;   //�����һ����ѹ״̬
		ADCMont_Flg.BAT_FULVOL = 0x01;
		return;
	}
	else 
	{
		ADCMont_Flg.BAT_FULVOL = 0;
	}
	
	if((Bat_HigVol <= ADC_Conversion_Value[4]) && (ADC_Conversion_Value[4] < Bat_FulVol))
	{
		*pADCFlg = (*pADCFlg) & 0x0f;
		ADCMont_Flg.BAT_HIGVOL = 0x01;
		return;
	}
	else
	{
		ADCMont_Flg.BAT_HIGVOL = 0;
	}
	
	if((Bat_MidVol <= ADC_Conversion_Value[4]) && (ADC_Conversion_Value[4] < Bat_HigVol))
	{
		*pADCFlg = (*pADCFlg) & 0x0f;
		ADCMont_Flg.BAT_MIDVOL = 0x01;
		return;
	}
	else
	{
		ADCMont_Flg.BAT_MIDVOL = 0;
	}
	
	if(((Bat_LowVol <= ADC_Conversion_Value[4]) && 
			(ADC_Conversion_Value[4] < Bat_MidVol)) || 
			(ADC_Conversion_Value[4] < Bat_LowVol))
	{
		*pADCFlg = (*pADCFlg) & 0x0f;
		ADCMont_Flg.BAT_LOWVOL = 0x01;
		return;
	}
	else
	{
		ADCMont_Flg.BAT_LOWVOL = 0;
	}
	
}


