#include "stm32f10x.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "sys.h" 
#include "Public_Value.h"

static void Stop_KEY_Init(void);
static u8 Get_KEY_State(u16 time);


/*******************************************************************
程序功能：KEY模块的回调函数，10ms task
程序版本：V1.0
日    期： 2019/6/10
作    者：Orange
修    改：无
*******************************************************************/

void MKEY_CallBack(void)
{
	KEY_Scan();
}

/*******************************************************************
程序功能：KEY模块的初始化
程序版本：V1.0
日    期： 2019/6/10
作    者：Orange
修    改：无
*******************************************************************/

void MKEY_Init(void)
{
	Stop_KEY_Init();
}

/*******************************************************************
程序功能：Motor_Stop_KEY初始化配置
程序版本：V1.0
日    期： 2019/6/10
作    者：Orange
修    改：无
*******************************************************************/

static void Stop_KEY_Init(void) 
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);   //使能PORTA时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;              
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);                
}

/*******************************************************************
程序功能：非阻塞式的按键识别
程序版本：V1.0
日    期： 2019/6/10
作    者：Orange
修    改：无
				  
*******************************************************************/

static u8 Get_KEY_State(u16 time)
{
    static u8 Key_State = N_KEY_State;         //按键状态初始化
    static u8 Key_Return = N_KEY;                //按键返回值初始化
    static u16 num = 0;
    
    switch(Key_State)
    {
        case N_KEY_State:
        {
            Key_Return = N_KEY;
            
            if(Motor_Stop_KEY == 0)
            {
                Key_State = S_KEY_State;                   //按键消抖，系统执行一个语句的时间
            }
            
            break;
        }
        
        case S_KEY_State:
        {
            if(Motor_Stop_KEY == 0)
            {
                Key_State = L_KEY_State;                //按键接着消抖，系统执行一个语句的时间
            }
            else
            {
                Key_State = N_KEY_State;
            }
            
            break;
        }
        
        case L_KEY_State:
        {        
            if((Motor_Stop_KEY == 1) && (Key_Return == N_KEY))        //防止按键长按释放后，误进入短按状态，所以加了一个Key_Return条件
            {
                Key_Return = S_KEY;
                Key_State = N_KEY_State;
                num = 0;
            }
            
            else if(Motor_Stop_KEY == 0)
            {
                num++;
                
                if(num >= time)                           //可以通过修改入口参数time的大小，来调整长按的时间，time = 20时，时长约1s
                {
                    Key_Return = L_KEY;
                    num = 0;
                }
		else
		{
			//do nothing;
		}
            }
            
            else if((Motor_Stop_KEY == 1) && (Key_Return == L_KEY))  //等待按键长按后释放
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
		Key_StopSts = 0x01;                      //短按电机停转
	}
	else if(L_KEY == Get_KEY_State(KDelay_Time)) 
	{
		Key_StopSts = 0x02;                      //长按电机启动
	}
	else
	{
		//测试
		//Key_StopSts = 0x00; 
	}
	
	return Key_StopSts;
}


	
