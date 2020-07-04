#include "stm32f10x.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "sys.h" 
#include "Public_Value.h"

static void Stop_KEY_Init(void);
static u8 Get_KEY_State(u16 time);


/*******************************************************************
�����ܣ�KEYģ��Ļص�������10ms task
����汾��V1.0
��    �ڣ� 2019/6/10
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

void MKEY_CallBack(void)
{
	KEY_Scan();
}

/*******************************************************************
�����ܣ�KEYģ��ĳ�ʼ��
����汾��V1.0
��    �ڣ� 2019/6/10
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

void MKEY_Init(void)
{
	Stop_KEY_Init();
}

/*******************************************************************
�����ܣ�Motor_Stop_KEY��ʼ������
����汾��V1.0
��    �ڣ� 2019/6/10
��    �ߣ�Orange
��    �ģ���
*******************************************************************/

static void Stop_KEY_Init(void) 
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);   //ʹ��PORTAʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;              
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //���ó���������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);                
}

/*******************************************************************
�����ܣ�������ʽ�İ���ʶ��
����汾��V1.0
��    �ڣ� 2019/6/10
��    �ߣ�Orange
��    �ģ���
				  
*******************************************************************/

static u8 Get_KEY_State(u16 time)
{
	static u8 Key_State = N_KEY_State;         //����״̬��ʼ��
    static u8 Key_Return = N_KEY;                //��������ֵ��ʼ��
    static u16 num = 0;
    
    switch(Key_State)
    {
        case N_KEY_State:
        {
            Key_Return = N_KEY;
            
            if(Motor_Stop_KEY == 0)
            {
                Key_State = S_KEY_State;                   //����������ϵͳִ��һ������ʱ��
            }
            
            break;
        }
        
        case S_KEY_State:
        {
            if(Motor_Stop_KEY == 0)
            {
                Key_State = L_KEY_State;                //��������������ϵͳִ��һ������ʱ��
            }
            else
            {
                Key_State = N_KEY_State;
            }
            
            break;
        }
        
        case L_KEY_State:
        {        
            if((Motor_Stop_KEY == 1) && (Key_Return == N_KEY))        //��ֹ���������ͷź������̰�״̬�����Լ���һ��Key_Return����
            {
                Key_Return = S_KEY;
                Key_State = N_KEY_State;
                num = 0;
            }
            
            else if(Motor_Stop_KEY == 0)
            {
                num++;
                
                if(num >= time)                           //����ͨ���޸���ڲ���time�Ĵ�С��������������ʱ�䣬time = 20ʱ��ʱ��Լ1s
                {
                    Key_Return = L_KEY;
                    num = 0;
                }
				else
				{
						//do nothing;
				}
            }
            
            else if((Motor_Stop_KEY == 1) && (Key_Return == L_KEY))  //�ȴ������������ͷ�
            {
                Key_State = N_KEY_State;              
            }
						
			else
			{
					//do nothing;
			}
            
            break;
        }
				
		default:
		{
				break;
		}
    }
    
    return Key_Return;
}


u8 KEY_Scan(void)
{
	static u8 Key_StopSts = 0;
	
	if(S_KEY == Get_KEY_State(KDelay_Time))
	{
		Key_StopSts = 0x01;                      //�̰����ͣת
	}
	else if(L_KEY == Get_KEY_State(KDelay_Time)) 
	{
		Key_StopSts = 0x02;                      //�����������
	}
	else
	{
		//����
		//Key_StopSts = 0x00; 
	}
	
	return Key_StopSts;
}


	
