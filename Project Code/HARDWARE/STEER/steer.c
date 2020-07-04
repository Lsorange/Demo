#include "steer.h"
#include "timer.h"
#include "sys.h"
#include "delay.h"
#include "math.h"
#include "Public_Value.h"

//u8 RSteer1_Angle;
//u8 RSteer2_Angle;

/********************************************************************
舵机型号：SG90
舵机参数：0.12秒/60度(4.8V) 0.002s/度（无负载）
位置等级：1024级
控制精度：2us/度
脉冲周期：20ms
--------------------------------------------
		 0.5ms-------------0度； 2.5%
		 1.0ms------------45度； 5.0%
         1.5ms------------90度； 7.5%  中位
         2.0ms-----------135度； 10.0%
         2.5ms-----------180度； 12.5%
--------------------------------------------
********************************************************************/

static void Steer_PosInit(void);
static u8 SteerInit_FinshFlg(void);
static u8 Analyze_StrData(u8 Str_Data);
static void Steer1_Control(u8 Pos_Cnt);
static void Steer2_Control(u8 Pos_Cnt);

enum Ang_Lel
{
	F_Lel  = 0x00,
	R_Lel1 = 0x01,
	R_Lel2 = 0x02,
	R_Lel3 = 0x04,
	R_Lel4 = 0x08,
	L_Lel1 = 0x10,
	L_Lel2 = 0x20,
	L_Lel3 = 0x40,
	L_Lel4 = 0x80,
}eStrSpd;

enum Ang
{
	F_Sts = 0x00,
	R_Sts1,
	R_Sts2,
	R_Sts3,
	R_Sts4,
	L_Sts1,
	L_Sts2,
	L_Sts3,
	L_Sts4,
};

/*******************************************************************
程序功能：舵机模块的回调函数，20ms task
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

void MSTEER_CallBack(void)
{
	Steer1_Control(Analyze_StrData(Recv_Data.RSteer1_Angle));
	Steer2_Control(Analyze_StrData(Recv_Data.RSteer2_Angle));
}


/*****************************************************************************
程序功能：舵机模块的初始化
程序版本：V1.0
入口参数：无
返回参数：无
日    期：2019/6/18
作    者：Orange
修    改：无
******************************************************************************/

void MSTEER_Init(void)
{
	Steer_PosInit();
	
	#if TEST_END
		Recv_Data.RSteer1_Angle = 0x08;
		Recv_Data.RSteer2_Angle = 0x04;
	#endif
}

/*****************************************************************************
程序功能：舵机模块初始化完成判断
程序版本：V1.0
入口参数：无
返回参数：Finshed_Flg
日    期：2019/6/18
作    者：Orange
修    改：无
******************************************************************************/

static u8 SteerInit_FinshFlg(void)
{
	static u8 Finshed_Flg = 0x00;
	static u8 time = 0x00;
	
	if(1 == Tsk_Flg.STRInit_Flg)
	{
		if(++time >= Delay_600ms)
		{
			Finshed_Flg = 0x01;
			time = 0x00;
		}
		else
		{
			//do nothing;
		}
		
		Tsk_Flg.STRInit_Flg = 0x00;
	}
	else
	{
		//do nothing;
	}
	
	return Finshed_Flg;
}


/*****************************************************************************
程序功能：解析控制舵机的数据
程序版本：V1.0
入口参数：Str_Data
返回参数：无
日    期：2019/6/18
作    者：Orange
修    改：无
******************************************************************************/

static u8 Analyze_StrData(u8 Str_Data)
{
	static u8 rn_value = 0x00;
	
	switch(Str_Data)
	{
		case F_Lel:
		{
			rn_value = F_Sts;
			break;
		}
		case R_Lel1:
		{
			rn_value = R_Sts1;
			break;
		}
		case R_Lel2:
		{
			rn_value = R_Sts2;
			break;
		}
		case R_Lel3:
		{
			rn_value = R_Sts3;
			break;
		}
		case R_Lel4:
		{
			rn_value = R_Sts4;
			break;
		}
		case L_Lel1:
		{
			rn_value = L_Sts1;
			break;
		}
		case L_Lel2:
		{
			rn_value = L_Sts2;
			break;
		}
		case L_Lel3:
		{
			rn_value = L_Sts3;
			break;
		}
		case L_Lel4:
		{
			rn_value = L_Sts4;
			break;
		}
		default:
		{
			break;
		}
	}
	
	return rn_value;
}


/*******************************************************************
程序功能： 舵机上电位置归中
程序版本： V1.0
日    期： 2019/5/30
作    者： Orange
修    改： 无
*******************************************************************/

static void Steer_PosInit(void)
{
	//系统给上电时，舵机处于中位
	TIM_SetCompare1(TIM3, 540 - 1);
	TIM_SetCompare2(TIM3, 540 - 1);
	//delay_ms(500);
}

/*******************************************************************
程序功能：舵机1控制
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

static void Steer1_Control(u8 Pos_Cnt)
{
	if(1 == SteerInit_FinshFlg())
	{
		if(Pos_Cnt <= R_Sts4)
		{
			TIM_SetCompare1(TIM3, 540 + (Pos_Cnt * 90) - 1);  //900对应180°，36对应9°，总共20细分
			//delay_ms(100);
		}
		else
		{
			TIM_SetCompare1(TIM3, 540 - ((Pos_Cnt-4) * 90) - 1);  //900对应180°，36对应9°，总共20细分
			//delay_ms(100);
		}
	}
	else
	{
		//do nothing;
	}
}

/*******************************************************************
程序功能：舵机2控制
程序版本：V1.0
日    期： 2019/5/30
作    者：Orange
修    改：无
*******************************************************************/

static void Steer2_Control(u8 Pos_Cnt)
{
	if(1 == SteerInit_FinshFlg())
	{
		if(Pos_Cnt <= R_Sts4)
		{
			TIM_SetCompare2(TIM3, 540 + (Pos_Cnt * 90) - 1);  //900对应180°，36对应9°，总共20细分
			//delay_ms(100);
		}
		else
		{
			TIM_SetCompare2(TIM3, 540 - ((Pos_Cnt-4) * 90) - 1);  //900对应180°，36对应9°，总共20细分
			//delay_ms(100);
		}
	}
	else
	{
		//do nothing;
	}
}

