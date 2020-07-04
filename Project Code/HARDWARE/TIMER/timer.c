#include "timer.h"
#include "sys.h"

/***********************************************************************
ȫ�ֱ�����bit����
------------------------------------------------------------------------
Name									bit					Defination
------------------------------------------------------------------------
Task_Flag	  							7					reserved
										6					USART_Control
										5					STEER_Control
										4					MOTOR_Control
										3					KEY_SCAN
										2					LED_Display
										1					ADC_Voltage
										0					ADC_Current
------------------------------------------------------------------------
************************************************************************/
Task_Flag Tsk_Flg;

static void TskFlg_Init(void);
static void TIM1_Init(u16 arr, u16 psc);
static void Turn_PWM_Init(u16 arr, u16 psc);
static void Motor_PWM_Init(u16 arr, u16 psc);

/*******************************************************************
�����ܣ���ʱ��ģ��ĳ�ʼ��
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

void MTIMER_Init(void)
{
	TskFlg_Init();
	TIM1_Init(TIMER_PRSC, TIMER_PERD);    //10ms
	Turn_PWM_Init(STEER_PRSC, STEER_PERD);
	Motor_PWM_Init(MOTOR_PRSC, MOTOR_PERD);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
}

/*******************************************************************
�����ܣ���ʱ����ѯ��־λ�ĳ�ʼ��
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void TskFlg_Init(void)
{
	u8 *pTimer = (u8*)&Tsk_Flg;
	u8 i;

	for(i = 0; i < sizeof(Task_Flag)/sizeof(u8); i++)
	{
		pTimer[i] = 0x00;
	}   
}

/*******************************************************************
�����ܣ��߼���ʱ��TIM1��ʼ��
����汾��V1.0
��    �ڣ� 2019/5/30 
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void TIM1_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM1��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//�ظ��������ã�����ֵ�������ٴμ����Ž����жϣ���ʱ����һ�ν����ж�
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);     //����жϱ�־λ����ֹ�����жϾͽ���һ���ж�
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMx
}

/*******************************************************************
�����ܣ��߼���ʱ��TIM1���жϴ�����
����汾��V1.0
��    �ڣ� 2019/5/30
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

void TIM1_UP_IRQHandler(void) 
{ 
	static u8 Tsk10ms_Cnt = 0;
	static u8 Tsk20ms_Cnt = 0;
	static u8 Tsk100ms_Cnt = 0;
	static u16 Tsk1000ms_Cnt = 0;
	
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//���TIMx���жϴ�����λ:TIM �ж�Դ 
		
		/****433Mģ��10ms����ʱ��ѯ��flag****/
		Tsk10ms_Cnt++;
		if(Tsk10ms_Cnt >= Task1_Cnt)
		{
			Tsk_Flg.USART_Control = 0x01;
			Tsk_Flg.KEY_SCAN = 0x01;
			Tsk10ms_Cnt = 0;
		}
		else
		{
			//do nothing
		}
		
		/*****���&���&LEDģ��20ms����ʱ��ѯ��flag*****/
		Tsk20ms_Cnt++;
		if(Tsk20ms_Cnt >= Task2_Cnt)
		{
			Tsk_Flg.MOTOR_Control = 0x01;
			Tsk_Flg.STEER_Control = 0x01;
			Tsk_Flg.STRInit_Flg = 0x01;
			Tsk_Flg.LED_Display = 0x01;
			Tsk20ms_Cnt = 0;
		}
		else
		{
			//do nothing
		}
		
		/****ADC������ģ��&����ģ��100ms����ʱ��ѯ��flag****/
		Tsk100ms_Cnt++;
		if(Tsk100ms_Cnt >= Task3_Cnt)
		{
			Tsk_Flg.ADC_Current  = 0x01;
			//Tsk_Flg.KEY_SCAN = 0x01;
			//Tsk_Flg.LED_Display = 0x01;
			Tsk100ms_Cnt = 0;
		}
		else
		{
			//do nothing
		}
		
		/****ADC����ѹģ��1000ms����ʱ��ѯ��flag****/
		Tsk1000ms_Cnt++;
		if(Tsk1000ms_Cnt >= Task4_Cnt)
		{
			Tsk_Flg.ADC_Voltage = 0x01;
			//Tsk_Flg.KEY_SCAN = 0x01;
			Tsk1000ms_Cnt = 0;
		}
		else
		{
			//do nothing
		}
	}	     
} 

/*******************************************************************
�����ܣ����PWM���Ƴ�ʼ��
����汾��V1.0
��    �ڣ� 2019/5/30
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void Turn_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //TIM3_CH1/TIM3_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                 //��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Եͣ�������ֵС��arrʱ������ߵ�ƽ������ʱ����͵�ƽ
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
}

/*******************************************************************
�����ܣ�ֱ�����PWM���Ƴ�ʼ��,ע��L298N���֧��Ƶ����40KHz
����汾��V1.0
��    �ڣ� 2019/5/30
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void Motor_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��GPIO����
	
	 //���TIM2 CHx��PWM���岨��	GPIOA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //TIM_CH1/TIM_CH2/TIM_CH3/TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
	
	 //��ʼ��TIM2
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM2 Channelx PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�,������ֵС��arrʱ������ߵ�ƽ������ʱ����͵�ƽ
	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2 OC1
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2 OC2
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2 OC3
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2 OC4

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR3�ϵ�Ԥװ�ؼĴ���
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR4�ϵ�Ԥװ�ؼĴ���
 
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2
}

