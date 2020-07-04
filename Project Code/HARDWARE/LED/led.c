#include "led.h"
#include "delay.h"
#include "Public_Value.h"

/*************************************************************************************************
LEDӲ�����������
Serial_Num    Name   						GPIO    INIT     Function
------------------------------------------------------------------------------------------------
    1         KEY_Sts_LED       			PA5     0(OFF)   ��ʾֹͣ������״̬������Ϊ1(ON)���̰�Ϊ0(OFF)
	2         MCU_Sts_LED       			PB5		1(ON)    ��Ƭ���ϵ�������������ϵ�Ϊ1(ON),�µ�Ϊ0(OFF)
	3		  Data_Sts_LED      			PB6     1(����)	 433M�ȴ�����ʱ������״̬�����ӳɹ�ʱ����״̬
	4		  Return_Sts_LED    			PB7     0(OFF)   ����ʱ����ʼ��������ʼ��ʱϨ��״̬
	5		  Front_Light_LED   			PB8     1(ON)	 ��Ƭ���ϵ������󣬿�ʼ����
	6         KEY_To_LED1					PC11	0(OFF)	 ��ʼ��ʱ��Ϩ�𣬵����յ�ң�����İ��������ź�ʱ����
	7		  KEY_To_LED2					PC12	0(OFF)	 ��ʼ��ʱ��Ϩ�𣬵����յ�ң�����İ��������ź�ʱ����
-------------------------------------------------------------------------------------------------
*************************************************************************************************/

/***********************************************************************
ȫ�ֱ�����bit����
------------------------------------------------------------------------
Name									bit					Defination
------------------------------------------------------------------------
RLED_State	  							7					reserved
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
�����ܣ�LEDģ��ص�������20ms task
����汾��V1.0
��    �ڣ� 2019/5/31
��    �ߣ�Orange
��    �ģ���
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
�����ܣ�LEDģ��ĳ�ʼ��
����汾��V1.0
��    �ڣ� 2019/5/31
��    �ߣ�Orange
��    �ģ���
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
�����ܣ�ȫ�ֱ���RLED_State�ĳ�ʼ��
����汾��V1.0
��    �ڣ� 2019/5/31
��    �ߣ�Orange
��    �ģ���
*************************************************************************/

static void RLEDSts_Init(void)
{
	ReadLed_Sts.DSts_LED = 0x01;   //ʵ��ֵ0x01 
	ReadLed_Sts.RSts_LED = 0x01;   //0x02
	ReadLed_Sts.FLight_LED = 0x01;  //0x04
	ReadLed_Sts.KEY_LED1 = 0x01;   //0x08
	ReadLed_Sts.KEY_LED2 = 0x01;   //0x10
}

/*************************************************************************
�����ܣ�LED��ʾ/LED����/�㰴LED��ʼ������
����汾��V1.0
��    �ڣ� 2019/5/31
��    �ߣ�Orange
��    �ģ���
*************************************************************************/

static void LED_Init(void)                                                                                                                                                                         
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);	      //ʹ��PB�˿�ʱ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		                                //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		                                //IO���ٶ�Ϊ50MHz
	
	//KEY_Sts_LED init
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;    
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				         
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);  		//�ϵ��Ĭ�ϣ������ת��������������������
	
	//MCU_Sts_LED & Data_Sts_LED & Return_Sts_LED & Front_Light_LED init
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8; //PB5-8 �˿�����
	GPIO_Init(GPIOB, &GPIO_InitStructure);					                                 //�����趨������ʼ��GPIOB
	GPIO_SetBits(GPIOB,GPIO_Pin_5);						                         			//PB5������ָʾ�ƣ�
	GPIO_ResetBits(GPIOB,GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);                       		//PB6(����״̬)��PB7(����ָʾ)��PB8(ǰ������)
	
	//KEY_To_LED1 & KEY_To_LED2 init
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;    //PC11-12 �˿�����, �������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				          //������� ��IO���ٶ�Ϊ50MHz
	GPIO_ResetBits(GPIOC, GPIO_Pin_11 | GPIO_Pin_12);           //PC11(�㰴LED1)��PC12(�㰴LED2)         
}

#if TEST_START

	void LED_Test(void)
	{

	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PE�˿�ʱ��

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6;	    		 //LED1-->PE.5 �˿�����, �������
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	 GPIO_SetBits(GPIOE,GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6); 						 //PE.5 ����� 
	}
	
#endif

/*************************************************************************
�����ܣ�����״̬���ӵ�LEDָʾ
����汾��V1.0
��    �ڣ� 2019/6/10
��    �ߣ�Orange
��    �ģ���
*************************************************************************/

void DataConnect_Sts(void)
{	
	//���ӳɹ�������״̬
	if(ReadLed_Sts.DSts_LED == (Recv_Data.RLED_State&0x01))
	{
		DATA_LED = 1;
	}
	else //�ȴ����ӣ�����״̬
	{
		if(0x01 == Tsk_Flg.LED_Display) 
		{
			DATA_LED = ~DATA_LED;
		}
	}
}

/*************************************************************************
�����ܣ�����״̬��LEDָʾ
����汾��V1.0
��    �ڣ� 2019/6/10
��    �ߣ�Orange
��    �ģ���
˵    �������ö�ʱ�����ƵƵ���������������ʱ����������ϵͳ��ʵʱ��
*************************************************************************/

static void Return_LED(void)
{
	static u8 cnt = 0;
	
	//�����źţ�����״̬,��������4s
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
�����ܣ������ͣ������ָʾ�ƣ����������������̰�����ֹͣ
����汾��V1.0
��    �ڣ� 2019/6/10
��    �ߣ�Orange
��    �ģ���
˵    ������
*************************************************************************/

static void KeyStp_LED(void)
{
	if(MOTOR_OFF == KEY_Scan())
	{
		KEY_Sts_LED = 0;		//���ֹͣ����Ϩ��
	}
	else if(MOTOR_ON == KEY_Scan())
	{
		KEY_Sts_LED = 1;		//����������Ƴ���
	}
	else
	{
		//do nothing
	}
}

/*************************************************************************
�����ܣ�ǰ��LED�������������ɿ�����
����汾��V1.0
��    �ڣ� 2019/6/10
��    �ߣ�Orange
��    �ģ���
˵    ������
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
�����ܣ��㰴LED�������������ɿ�����
����汾��V1.0
��    �ڣ� 2019/6/10
��    �ߣ�Orange
��    �ģ���
˵    ������
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
