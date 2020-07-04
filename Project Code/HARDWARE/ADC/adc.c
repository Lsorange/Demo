#include "adc.h"
#include "sys.h"
#include "delay.h"
#include "Public_Value.h"

/***********************************************************************
全局变量的bit分配
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
u16 ADC_Conversion_Value[5];       //ADC采集值存储的数组

static void ADCFlg_Init(void);
static void ADC1_GPIO_Config(void);
static void ADC1_Config(void);
static void ADC1_DMA_Config(void);
static void ADC_Convert(void);
static void Cur_Montr(void);
static void Vol_Montr(void);


/*******************************************************************
程序功能：电流采集的回调函数，100ms task
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

void MCUR_CallBack(void)
{
	ADC_Convert();
	Cur_Montr();
}

/*******************************************************************
程序功能：电压采集的回调函数，1000ms task
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

void MVOL_CallBack(void)
{
	ADC_Convert();
	Vol_Montr();
}

/*******************************************************************
程序功能：ADC模块的初始化
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

void MADC_Init(void)
{
	ADCFlg_Init();
	ADC1_GPIO_Config();
	ADC1_Config();
	ADC1_DMA_Config();
}

/*******************************************************************
程序功能：电压电流的初始化
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
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
程序功能：ADC1的GPIO配置
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
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
程序功能：ADC1配置
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

static void ADC1_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  //使能ADC1通道时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	ADC_DeInit(ADC1);  //复位ADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	      //模数转换工作在扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 5;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器 

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_239Cycles5);
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);	    //使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
	
}

/*******************************************************************
程序功能：ADC1_DMA配置
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

static void ADC1_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
	DMA_DeInit(DMA1_Channel1);   //将DMA的通道1寄存器重设为缺省值

	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADC1 -> DR));  //DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(ADC_Conversion_Value[0]);  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设到内存
	DMA_InitStructure.DMA_BufferSize = 5;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;      //数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

/*******************************************************************
程序功能：ADC采集值转换
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

static void ADC_Convert(void)
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

/*******************************************************************
程序功能：电机电流的监测
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

static void Cur_Montr(void)
{
	static u8 M1_Cnt, M2_Cnt, M3_Cnt, M4_Cnt;
	
	//检测M1电机的当前电流状态
	if(ADC_Conversion_Value[0] > Over_Current_Volatge)
	{
		M1_Cnt++;
		if(M1_Cnt >= FILT_CNT)  //滤波处理
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
	
	//检测M2电机的当前电流状态
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
	
	//检测M3电机的当前电流状态
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
	
	//检测M4电机的当前电流状态
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
程序功能：蓄电池电压的监测
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

static void Vol_Montr(void)
{
	/****检测蓄电池电压的状态****/
	if(ADC_Conversion_Value[4] >= Bat_FulVol)
	{
		*pADCFlg = (*pADCFlg) & 0x0f;   //清除上一个电压状态
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


