#include "timer.h"
#include "sys.h"

/***********************************************************************
全局变量Tsk_Flg
************************************************************************/
Task_Flag Tsk_Flg;

static void TskFlg_Init(void);
static void TIM1_Init(u16 arr, u16 psc);
static void Turn_PWM_Init(u16 arr, u16 psc);
static void Motor_PWM_Init(u16 arr, u16 psc);

/*******************************************************************
程序功能：定时器模块的初始化
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

void MTIMER_Init(void)
{
	TskFlg_Init();
	TIM1_Init(TIMER_PRSC, TIMER_PERD);    //10ms
	Turn_PWM_Init(STEER_PRSC, STEER_PERD);
	Motor_PWM_Init(MOTOR_PRSC, MOTOR_PERD);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}

/*******************************************************************
程序功能：定时器轮询标志位的初始化
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
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
程序功能：高级定时器TIM1初始化
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

static void TIM1_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//重复计数设置，这里值决定多少次计数才进入中断，此时计数一次进入中断
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);     //清除中断标志位，防止开启中断就进入一次中断
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM1, ENABLE);  //使能TIMx
}

/*******************************************************************
程序功能：高级定时器TIM1的中断处理函数
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

void TIM1_UP_IRQHandler(void) 
{ 
	static u8 Tsk10ms_Cnt = 0;
	static u8 Tsk20ms_Cnt = 0;
	static u8 Tsk100ms_Cnt = 0;
	static u16 Tsk1000ms_Cnt = 0;
	
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源 
		
		/****433M模块10ms任务定时轮询的flag****/
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
		
		/*****电机&舵机&LED模块20ms任务定时轮询的flag*****/
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
		
		/****ADC监测电流模块&按键模块100ms任务定时轮询的flag****/
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
		
		/****ADC监测电压模块1000ms任务定时轮询的flag****/
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
程序功能：舵机PWM控制初始化
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

static void Turn_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIO外设和AFIO复用功能模块时钟
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //TIM3_CH1/TIM3_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                 //初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低，当计数值小于arr时，输出高电平，大于时输出低电平
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
}

/*******************************************************************
程序功能：直流电机PWM控制初始化,注意L298N最大支持频率是40KHz
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

static void Motor_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIO外设
	
	 //输出TIM2 CHx的PWM脉冲波形	GPIOA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //TIM_CH1/TIM_CH2/TIM_CH3/TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
	
	 //初始化TIM2
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM2 Channelx PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低,当计数值小于arr时，输出高电平，大于时输出低电平
	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC1
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC2
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC3
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC4

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR1上的预装载寄存器
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR2上的预装载寄存器
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR3上的预装载寄存器
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR4上的预装载寄存器
 
	TIM_Cmd(TIM2, ENABLE);  //使能TIM2
}

