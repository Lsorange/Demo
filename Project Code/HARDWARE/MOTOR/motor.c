#include "motor.h"
#include "sys.h"
#include "timer.h"
#include "Public_Value.h"

/*********************************************************************************************
全局变量的bit分配
----------------------------------------------------------------------------------------------
Name				bit				Defination
----------------------------------------------------------------------------------------------
RMainMotor_State		7				reserved        //Motor3Cur_Sts
				6				Stop_State      //Motor2Cur_Sts
				5				Forward_State
				4				Reverse_State
				3				reserved	//Stop_State
				2				Init_State
				1				reserved        //Motor1Cur_Sts
				0				MainMotor_Num
----------------------------------------------------------------------------------------------
RExtdMotor_State		7				reserved
				6				Stop_State
				5				Forward_State
				4				Reverse_State
				3				reserved        //Stop_State
				2				Init_State
				1				ExtdMotor_Num
				0				reserved        //Motor1Cur_Sts
----------------------------------------------------------------------------------------------
RMainMotor_Spd			7				reserved        //Spd_Level6
				6				Spd_Level5
				5				Spd_Level4
				4				Spd_Level3
				3				Spd_Level2
				2				Spd_Level1
				1				Spd_Level0
RExtdMotor_Spd			0				SpdMode_State
----------------------------------------------------------------------------------------------
**********************************************************************************************/
//u8 RMainMotor_State = 0;
//u8 RExtdMotor_State = 0;
//u8 RMainMotor_Spd = 0;
//u8 RExtdMotor_Spd = 0;

static u8 Read_MotorNum(void);
static u8 Read_MotorMode(u8 Motor_State);
static u8 Read_MotorSpd(u8 Motor_Spd);
static u8 Read_MotorFlg(u8 Motor_Flg);
static u8 Read_MotorCurSts(u8 Cur_Sts, u8 Motor_Type);
static void Motor_Control(void);
static void MotorDir_Init(void);
static void MainMotor_Control(u16 time, u8 mode, u8 speed_mode);
static void ExtdMotor_Control(u16 time, u8 mode, u8 speed_mode);
static void MainMotor_Init(void);
static void ExtdMotor_Init(void);
static void MainMotor_SpdMode(u8 spd_level);
static void ExtdMain_SpdMode(u8 spd_level);
static void MainMotor_Forward(void);
static void ExtdMotor_Forward(void);
static void MainMotor_Reverse(void);
static void ExtdMotor_Reverse(void);

/*************************电机控制状态定义******************************/
enum State
{
	SpeedMode_State = 0x01,
	MainMotor1_CurSts = 0x01,
	MainMotor2_CurSts = 0x02,
	MainMotor3_CurSts = 0x04,
	ExtdMotor_CurSts = 0x08,
	Init_State = 0x04,
	Reverse_State = 0x10,
	Forward_State = 0x20,
	Stop_State = 0x40,
};

enum Num
{
	MainMotor_Num = 1,
	ExtdMotor_Num,
};

/*******************************************************************
程序功能：电机模块的回调函数，20ms task
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

void MMotor_CallBack(void)
{
	Motor_Control();
}

/*******************************************************************
程序功能：电机模块的初始化
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

void MMotor_Init(void)
{
	MotorDir_Init();
	MainMotor_Init();
	ExtdMotor_Init();
}

/*******************************************************************
程序功能：解析从433M接收的信息
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

/*****读取电机的序列号*****/
static u8 Read_MotorNum(void)
{
	u8 Ser_Num = 0;
	
	Ser_Num = (Recv_Data.RMainMotor_State & 0xFF) | (Recv_Data.RExtdMotor_State & 0xFF);
	
	return Ser_Num;
}

/*****读取电机的状态*****/
static u8 Read_MotorMode(u8 Motor_State)
{
	u8 R_Mode = 0;
	
	R_Mode = (Motor_State&0x04) | (Motor_State&0x40) |
			 (Motor_State&0x10) | (Motor_State&0x20); //init state | stop state | forword state | reverse state
	
	return R_Mode;
}

/*****读取电机的速度挡位*****/
static u8 Read_MotorSpd(u8 Motor_Spd)
{
	u8 R_Spd = 0;
	u8 i;
	
	for(i = 1; i < 8; i++)
	{
		if(0x01 == (Motor_Spd >> i))
		{
			R_Spd = (0x01 << i) | 0x01;  //若有速度信号，同时使能速度模式
			break;
		}
	}
	
	return R_Spd;
}

/*****读取电机的切换状态*****/
static u8 Read_MotorFlg(u8 Motor_Flg)
{
	u8 Switch_Sts = 0;
	static u8 Temp_Flag = 0x30;
	
	Motor_Flg = Motor_Flg & 0x30;                     //读取切换状态只针对正反转切换，过滤掉初始化和停止状态
	if(Motor_Flg != (Motor_Flg&Temp_Flag))
	{
		Switch_Sts = 0x02;
	}
	else
	{
		Switch_Sts = 0x00;
	}
	
	Temp_Flag = Motor_Flg;
	
	return Switch_Sts;
}

/*****读取电机的电流状态*****/
static u8 Read_MotorCurSts(u8 Cur_Sts, u8 Motor_Type)
{
	u8 MotorCur_State = 0;
	
	//主电机
	if(Main_Type == Motor_Type)
	{
		MotorCur_State = ((Cur_Sts&0x01) | (Cur_Sts&0x02) | (Cur_Sts&0x04));
	}
	else  //扩展电机
	{
		MotorCur_State = (Cur_Sts&0x08);
	}
	
	return MotorCur_State;
}

////////////////////////////////////////////////////////////////////////////

/*******************************************************************
程序功能：电机控制
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/
static void Motor_Control(void)
{
	u8 Serial_Num = Read_MotorNum();
	u8 MainMotor_Mode = Read_MotorMode(Recv_Data.RMainMotor_State);
	u8 MainMotor_Spd = Read_MotorSpd(Recv_Data.RMainMotor_Spd);
	u8 ExtdMotor_Mode = Read_MotorMode(Recv_Data.RExtdMotor_State);
	u8 ExtdMotor_Spd = Read_MotorSpd(Recv_Data.RExtdMotor_Spd);
	
	//主电机的序列号
	if(MainMotor_Num == (Serial_Num&0x01))
	{
		MainMotor_Control(MDelay_Time, MainMotor_Mode, MainMotor_Spd);
	}

	//扩展电机的序列号
	if(ExtdMotor_Num == (Serial_Num&0x02))
	{
		ExtdMotor_Control(MDelay_Time, ExtdMotor_Mode, ExtdMotor_Spd);
	}
}



/*******************************************************************
程序功能：直流电机方向控制初始化
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

static void MotorDir_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	      //使能PB端口时钟
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		                                //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		                                //IO口速度为50MHz
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; //PB12-15 端口配置
	GPIO_Init(GPIOB, &GPIO_InitStructure);					                                 //根据设定参数初始化GPIOB
	GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);    //PB12(IN_1A)、PB13(IN_1B)、PB14(IN_2A)、PB15(IN_2B)

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; //PC6-9 端口配置, 推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				                               //推挽输出 ，IO口速度为50MHz
	GPIO_ResetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);        //PC6(IN_3A)、PC7(IN_3B)、PC8(IN_4A)、PC9(IN_4B)
}


/****************************************************************************************
程序功能：直流主电机控制，同时支持调速和正反转模式
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/

static void MainMotor_Control(u16 time, u8 mode, u8 speed_mode)
{
	/*****************************************************************
	标志位的位定义
	------------------------------------------------------------------
	Name				Flag         bit      value			Func
	------------------------------------------------------------------
	Sts_Flg				Direct_Flg    0        0x01			无需延时直接进入其他状态
						Switch_Flg    1        0x02     	正反转的切换状态
	*****************************************************************/
	static u16 num = 0;
	static u8 MainSts_Flg = 0;  //从stop_state直接切换到Forward_State、Reverse_State
	
	MainSts_Flg = Read_MotorFlg(mode);
	
	if(MOTOR_ST == KEY_Scan())
	{
		//测试
		//KEY_Test_LED = 0;
		
		//电机处于free running状态
		if((Init_State == (mode&0x04)) || (Stop_State == (mode&0x40)) || (0x02 == (MainSts_Flg&0xff)))
		{
			MainMotor_Init();
			MainSts_Flg |= 0x01;
		}
		else if((++num >= time) || (1 == (MainSts_Flg&0x03))) //当电机减速到一定的程度，即可切换旋转方向或其实状态为停止状态
		{
			if(SpeedMode_State == (speed_mode&0x01))
			{
				MainMotor_SpdMode(speed_mode&0xfe);
			}
			else
			{
				//do nothing
			}
			
			if(Forward_State == (mode&0x20))
			{
				MainMotor_Forward();
				num = 0;
				MainSts_Flg = 0;
			}
			else if(Reverse_State == (mode&0x10))
			{
				MainMotor_Reverse();
				num = 0;
				MainSts_Flg = 0;
			}
			else
			{
				//do nothing
			}
		}
	}
	else
	{
		MainMotor_Init();
	}
}

/****************************************************************************************
程序功能：扩展电机控制，同时支持调速和正反转模式
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/

static void ExtdMotor_Control(u16 time, u8 mode, u8 speed_mode)
{
	static u8 num = 0;
	static u8 ExtdSts_Flg = 0;
	
	ExtdSts_Flg = Read_MotorFlg(mode);
	
	if(MOTOR_ST == KEY_Scan())
	{
		//电机处于free running状态
		if((Init_State == (mode&0x04)) || (Stop_State == (mode&0x40)) || (0x02 == (ExtdSts_Flg&0xff)))
		{
			ExtdMotor_Init();
			ExtdSts_Flg |= 1;
		}
		else if((++num >= time) || (1 == (ExtdSts_Flg&0x03))) 
		{
			if(SpeedMode_State == (speed_mode&0x01))
			{
				ExtdMain_SpdMode(speed_mode&0xfe);
			}
			else
			{
				//do nothing
			}
		
			if(Forward_State == (mode&0x20))
			{		
				ExtdMotor_Forward();
				num = 0;
				ExtdSts_Flg = 0;
			}
			else if(Reverse_State == (mode&0x10))
			{
				ExtdMotor_Reverse();
				num = 0;
				ExtdSts_Flg = 0;
			}
			else
			{
				//do nothing
			}
		}
	}
	else
	{
		ExtdMotor_Init();
	}
}


/****************************************************************************************
程序功能：直流主电机初始化或停止
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/

static void MainMotor_Init(void)
{
	//Disable EN_Pin
	TIM_SetCompare1(TIM2, 0);
	TIM_SetCompare2(TIM2, 0);
	TIM_SetCompare3(TIM2, 0);
	
	//Init Dir_Pin
	IN_1A = 0;
	IN_1B = 0;
	IN_2A = 0;
	IN_2B = 0;
	IN_3A = 0;
	IN_3B = 0;
}

/****************************************************************************************
程序功能：直流扩展电机初始化或停止
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/

static void ExtdMotor_Init(void)
{
	TIM_SetCompare4(TIM2, 0);
	IN_4A = 0;
	IN_4B = 0;
}

/****************************************************************************************
程序功能：直流主电机调速
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/

static void MainMotor_SpdMode(u8 spd_level)
{
	u8 i;
	
	for(i = 1; i < 8; i++)
	{
		if(0x01 == (spd_level >> i))
		{
			spd_level = i;  //若有速度挡位的计算
			break;
		}
	}
	
	TIM_SetCompare1(TIM2, (Basic_SpdVal*spd_level) - 1);
	TIM_SetCompare2(TIM2, (Basic_SpdVal*spd_level) - 1);
	TIM_SetCompare3(TIM2, (Basic_SpdVal*spd_level) - 1);
}

/****************************************************************************************
程序功能：直流扩展电机调速
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/
static void ExtdMain_SpdMode(u8 spd_level)
{
	u8 i;
	
	for(i = 1; i < 8; i++)
	{
		if(0x01 == (spd_level >> i))
		{
			spd_level = i;  //若有速度挡位的计算
			break;
		}
	}
	
	TIM_SetCompare4(TIM2, (Basic_SpdVal*spd_level) - 1);
}


/****************************************************************************************
程序功能：直流主电机正转，支持过流保护
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/

static void MainMotor_Forward(void)
{
	u8 Cur_State = Read_MotorCurSts(*pADCFlg, Main_Type);
	
	if(0 == (Cur_State & MainMotor1_CurSts))
	{
		IN_1A = 1;
		IN_1B = 0;
	}
	else
	{
		TIM_SetCompare1(TIM2, 0);
		IN_1A = 0;
		IN_1B = 0;
	}
	
	if(0 == (Cur_State & MainMotor2_CurSts))
	{
		IN_2A = 1;
		IN_2B = 0;
	}
	else
	{
		TIM_SetCompare2(TIM2, 0);
		IN_2A = 0;
		IN_2B = 0;
	}
	
	if(0 == (Cur_State & MainMotor3_CurSts))
	{
		IN_3A = 1;
		IN_3B = 0;
	}
	else
	{
		TIM_SetCompare3(TIM2, 0);
		IN_3A = 0;
		IN_3B = 0;
	}
}

/****************************************************************************************
程序功能：直流扩展电机正转，支持过流保护
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/

static void ExtdMotor_Forward(void)
{
	u8 Cur_State = Read_MotorCurSts(*pADCFlg, Extd_Type);  //*pADCFlg传递ADC模块采集的电流值，直接传值
	
	if(0 == (Cur_State & ExtdMotor_CurSts))
	{
		IN_4A = 1;
		IN_4B = 0;
	}
	else
	{
		TIM_SetCompare4(TIM2, 0);
		IN_4A = 0;
		IN_4B = 0;
	}
}

/****************************************************************************************
程序功能：直流主电机反转，支持过流保护
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/

static void MainMotor_Reverse(void)
{
	u8 Cur_State = Read_MotorCurSts(*pADCFlg, Main_Type);
	
	if(0 == (Cur_State & MainMotor1_CurSts))
	{
		IN_1A = 0;
		IN_1B = 1;
	}
	else
	{
		TIM_SetCompare1(TIM2, 0);
		IN_1A = 0;
		IN_1B = 0;
	}
	
	if(0 == (Cur_State & MainMotor2_CurSts))
	{
		IN_2A = 0;
		IN_2B = 1;
	}
	else
	{
		TIM_SetCompare2(TIM2, 0);
		IN_2A = 0;
		IN_2B = 0;
	}
	
	if(0 == (Cur_State & MainMotor3_CurSts))
	{
		IN_3A = 0;
		IN_3B = 1;
	}
	else
	{
		TIM_SetCompare3(TIM2, 0);
		IN_3A = 0;
		IN_3B = 0;
	}
}

/****************************************************************************************
程序功能：直流扩展电机反转，支持过流保护
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
****************************************************************************************/

static void ExtdMotor_Reverse(void)
{
	u8 Cur_State = Read_MotorCurSts(*pADCFlg, Extd_Type);
	
	if(0 == (Cur_State & ExtdMotor_CurSts))
	{
		IN_4A = 0;
		IN_4B = 1;
	}
	else
	{
		TIM_SetCompare4(TIM2, 0);
		IN_4A = 0;
		IN_4B = 0;
	}
}




