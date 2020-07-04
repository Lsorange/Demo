#include "steer.h"
#include "timer.h"
#include "sys.h"
#include "delay.h"
#include "math.h"
#include "Public_Value.h"

//u8 RSteer1_Angle;
//u8 RSteer2_Angle;

/********************************************************************
����ͺţ�SG90
���������0.12��/60��(4.8V) 0.002s/�ȣ��޸��أ�
λ�õȼ���1024��
���ƾ��ȣ�2us/��
�������ڣ�20ms
--------------------------------------------
		 0.5ms-------------0�ȣ� 2.5%
		 1.0ms------------45�ȣ� 5.0%
         1.5ms------------90�ȣ� 7.5%  ��λ
         2.0ms-----------135�ȣ� 10.0%
         2.5ms-----------180�ȣ� 12.5%
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
�����ܣ����ģ��Ļص�������20ms task
����汾��V1.0
��    �ڣ� 2019/5/30
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

void MSTEER_CallBack(void)
{
	Steer1_Control(Analyze_StrData(Recv_Data.RSteer1_Angle));
	Steer2_Control(Analyze_StrData(Recv_Data.RSteer2_Angle));
}


/*****************************************************************************
�����ܣ����ģ��ĳ�ʼ��
����汾��V1.0
��ڲ�������
���ز�������
��    �ڣ�2019/6/18
��    �ߣ�Orange
��    �ģ���
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
�����ܣ����ģ���ʼ������ж�
����汾��V1.0
��ڲ�������
���ز�����Finshed_Flg
��    �ڣ�2019/6/18
��    �ߣ�Orange
��    �ģ���
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
�����ܣ��������ƶ��������
����汾��V1.0
��ڲ�����Str_Data
���ز�������
��    �ڣ�2019/6/18
��    �ߣ�Orange
��    �ģ���
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
�����ܣ� ����ϵ�λ�ù���
����汾�� V1.0
��    �ڣ� 2019/5/30
��    �ߣ� Orange
��    �ģ� ��
*******************************************************************/

static void Steer_PosInit(void)
{
	//ϵͳ���ϵ�ʱ�����������λ
	TIM_SetCompare1(TIM3, 540 - 1);
	TIM_SetCompare2(TIM3, 540 - 1);
	//delay_ms(500);
}

/*******************************************************************
�����ܣ����1����
����汾��V1.0
��    �ڣ� 2019/5/30
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void Steer1_Control(u8 Pos_Cnt)
{
	if(1 == SteerInit_FinshFlg())
	{
		if(Pos_Cnt <= R_Sts4)
		{
			TIM_SetCompare1(TIM3, 540 + (Pos_Cnt * 90) - 1);  //900��Ӧ180�㣬36��Ӧ9�㣬�ܹ�20ϸ��
			//delay_ms(100);
		}
		else
		{
			TIM_SetCompare1(TIM3, 540 - ((Pos_Cnt-4) * 90) - 1);  //900��Ӧ180�㣬36��Ӧ9�㣬�ܹ�20ϸ��
			//delay_ms(100);
		}
	}
	else
	{
		//do nothing;
	}
}

/*******************************************************************
�����ܣ����2����
����汾��V1.0
��    �ڣ� 2019/5/30
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void Steer2_Control(u8 Pos_Cnt)
{
	if(1 == SteerInit_FinshFlg())
	{
		if(Pos_Cnt <= R_Sts4)
		{
			TIM_SetCompare2(TIM3, 540 + (Pos_Cnt * 90) - 1);  //900��Ӧ180�㣬36��Ӧ9�㣬�ܹ�20ϸ��
			//delay_ms(100);
		}
		else
		{
			TIM_SetCompare2(TIM3, 540 - ((Pos_Cnt-4) * 90) - 1);  //900��Ӧ180�㣬36��Ӧ9�㣬�ܹ�20ϸ��
			//delay_ms(100);
		}
	}
	else
	{
		//do nothing;
	}
}

