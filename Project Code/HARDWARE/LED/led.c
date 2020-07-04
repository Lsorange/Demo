#include "led.h"
#include "delay.h"
#include "Public_Value.h"

/***************************************************************************************************************
LED硬件层的索引：
Serial_Num    		Name   			GPIO    INIT     Function
--------------------------------------------------------------------------------------------------------------
    1         		KEY_Sts_LED       	PA5     0(OFF)   显示停止按键的状态，长按为1(ON)，短按为0(OFF)
    2         		MCU_Sts_LED       	PB5	1(ON)    单片机上电正常后点亮，上电为1(ON),下电为0(OFF)
    3		  	Data_Sts_LED      	PB6     1(快闪)	 433M等待连接时，快闪状态，连接成功时常亮状态
    4		  	Return_Sts_LED    	PB7     0(OFF)   返航时，开始慢闪，初始化时熄灭状态
    5			Front_Light_LED   	PB8     1(ON)	 单片机上电正常后，开始点亮
    6         		KEY_To_LED1		PC11	0(OFF)	 初始化时，熄灭，当接收到遥控器的按键按下信号时点亮
    7		  	KEY_To_LED2		PC12	0(OFF)	 初始化时，熄灭，当接收到遥控器的按键按下信号时点亮
--------------------------------------------------------------------------------------------------------------
***************************************************************************************************************/

/***********************************************************************
全局变量的bit分配
------------------------------------------------------------------------
Name		  bit					Defination
------------------------------------------------------------------------
RLED_State	  7					reserved
		  6					reserved
		  5					reserved
		  4					KEY_LED2
		  3					KEY_LED1
		  2					FrontLight_LED
		  1					ReturnSts_LED
		  0					DataSts_LED
------------------------------------------------------------------------
************************************************************************/
//u8 RLED_State = 0;

static void RLEDSts_Init(void);
static void LED_Init(void);  
static void Return_LED(void);
static void KeyStp_LED(void);
static void KeyFrnt_LED(void);
static void KEY_To_LED(void);

struct ReadLed_State
{
	u8 DSts_LED : 1;
	u8 RSts_LED : 1;
	u8 FLight_LED : 1;
	u8 KEY_LED1 : 1;
	u8 KEY_LED2 : 1;
}ReadLed_Sts;

//u8 *pRSts = (u8*)&ReadLed_Sts;

/*************************************************************************
程序功能：LED模块回调函数，20ms task
程序版本：V1.0
日    期： 2019/5/31
作    者：Orange
修    改：无
*************************************************************************/

void MLED_CallBack(void)
{
	#if TEST_END
		#define T_LED_Start
		#include "test.h"
	#endif
	
	DataConnect_Sts(); 
	Return_LED();
	KeyStp_LED();
	KeyFrnt_LED();
	KEY_To_LED();	
	
	#if TEST_END
		#define T_LED_End
		#include "test.h"
	#endif
}

/*************************************************************************
程序功能：LED模块的初始化
程序版本：V1.0
日    期： 2019/5/31
作    者：Orange
修    改：无
*************************************************************************/

void MLED_Init(void)
{
	RLEDSts_Init();
	LED_Init();
	
	#if TEST_START
		LED_Test();
	#endif
}

/*************************************************************************
程序功能：全局变量RLED_State的初始化
程序版本：V1.0
日    期： 2019/5/31
作    者：Orange
修    改：无
*************************************************************************/

static void RLEDSts_Init(void)
{
	ReadLed_Sts.DSts_LED = 0x01;   //实际值0x01 
	ReadLed_Sts.RSts_LED = 0x01;   //0x02
	ReadLed_Sts.FLight_LED = 0x01;  //0x04
	ReadLed_Sts.KEY_LED1 = 0x01;   //0x08
	ReadLed_Sts.KEY_LED2 = 0x01;   //0x10
}

/*************************************************************************
程序功能：LED显示/LED照明/点按LED初始化配置
程序版本：V1.0
日    期： 2019/5/31
作    者：Orange
修    改：无
*************************************************************************/

static void LED_Init(void)                                                                                                                                                                         
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);	      //使能PB端口时钟
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		                                //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		                                //IO口速度为50MHz
	
	//KEY_Sts_LED init
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;    
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				         
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);  		//上电后默认，电机不转动，长按按键后电机启动
	
	//MCU_Sts_LED & Data_Sts_LED & Return_Sts_LED & Front_Light_LED init
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8; //PB5-8 端口配置
	GPIO_Init(GPIOB, &GPIO_InitStructure);					                                 //根据设定参数初始化GPIOB
	GPIO_SetBits(GPIOB,GPIO_Pin_5);						                         			//PB5（工作指示灯）
	GPIO_ResetBits(GPIOB,GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);                       		//PB6(数据状态)、PB7(返航指示)、PB8(前照明灯)
	
	//KEY_To_LED1 & KEY_To_LED2 init
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;    //PC11-12 端口配置, 推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				          //推挽输出 ，IO口速度为50MHz
	GPIO_ResetBits(GPIOC, GPIO_Pin_11 | GPIO_Pin_12);           //PC11(点按LED1)、PC12(点按LED2)         
}

#if TEST_START

	void LED_Test(void)
	{

	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6;	    		 //LED1-->PE.5 端口配置, 推挽输出
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	 GPIO_SetBits(GPIOE,GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6); 						 //PE.5 输出高 
	}
	
#endif

/*************************************************************************
程序功能：数据状态连接的LED指示
程序版本：V1.0
日    期： 2019/6/10
作    者：Orange
修    改：无
*************************************************************************/

void DataConnect_Sts(void)
{	
	//连接成功，常亮状态
	if(ReadLed_Sts.DSts_LED == (Recv_Data.RLED_State&0x01))
	{
		DATA_LED = 1;
	}
	else //等待连接，快闪状态
	{
		if(0x01 == Tsk_Flg.LED_Display) 
		{
			DATA_LED = ~DATA_LED;
		}
	}
}

/*************************************************************************
程序功能：返航状态的LED指示
程序版本：V1.0
日    期： 2019/6/10
作    者：Orange
修    改：无
说    明：采用定时来控制灯的慢闪，避免用延时函数减低了系统的实时性
*************************************************************************/

static void Return_LED(void)
{
	static u8 cnt = 0;
	
	//返航信号，慢闪状态,慢闪周期4s
	if(ReadLed_Sts.RSts_LED == ((Recv_Data.RLED_State>>1)&0x01) || 
	   1 == ADCMont_Flg.BAT_LOWVOL)
	{
		if(0x01 == Tsk_Flg.LED_Display)  
		{
			cnt++;
		}
		if(cnt >= 200)
		{
			RETURN_LED = ~RETURN_LED;
			cnt = 0;
		}
	}
	else
	{
		RETURN_LED = 0;
	}
}

/*************************************************************************
程序功能：电机启停按键的指示灯，长按灯亮启动，短按灯灭停止
程序版本：V1.0
日    期： 2019/6/10
作    者：Orange
修    改：无
说    明：无
*************************************************************************/

static void KeyStp_LED(void)
{
	if(MOTOR_OFF == KEY_Scan())
	{
		KEY_Sts_LED = 0;		//电机停止，灯熄灭
	}
	else if(MOTOR_ON == KEY_Scan())
	{
		KEY_Sts_LED = 1;		//电机启动，灯常亮
	}
	else
	{
		//do nothing
	}
}

/*************************************************************************
程序功能：前照LED，长按灯亮，松开灯灭
程序版本：V1.0
日    期： 2019/6/10
作    者：Orange
修    改：无
说    明：无
*************************************************************************/

static void KeyFrnt_LED(void)
{
	if(ReadLed_Sts.FLight_LED == ((Recv_Data.RLED_State>>2)&0x01))
	{
		FORWARD_LED = 1;
	}
	else
	{
		FORWARD_LED = 0;
	}
}

/*************************************************************************
程序功能：点按LED，长按灯亮，松开灯灭
程序版本：V1.0
日    期： 2019/6/10
作    者：Orange
修    改：无
说    明：无
*************************************************************************/

static void KEY_To_LED(void)
{
	if(ReadLed_Sts.KEY_LED1 == ((Recv_Data.RLED_State>>3)&0x01))
	{
		JOG_LED1 = 1;
	}
	else
	{
		JOG_LED1 = 0;
	}
	
	if(ReadLed_Sts.KEY_LED2 == ((Recv_Data.RLED_State>>4)&0x01))
	{
		JOG_LED2 = 1;
	}
	else
	{
		JOG_LED2 = 0;
	}
}
