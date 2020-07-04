#include "sys.h"
#include "usart.h"
#include "433m.h"
#include "led.h"
#include "Public_Value.h"

/*******************************************
USART数据接收完成标志位：RX_Flg
变量类型：global volatile value
申明位置：Public_Value.h
********************************************/
volatile u8 RX_Flg = 0;
volatile u8 Data_Len = 0;

/****************************************************************************************
程序功能：USART模块的初始化
程序版本：V1.0
日    期：2019/6/18
作    者：Orange
修    改：无
****************************************************************************************/

void MUSART_Init(void)
{
	usart_init(BOUND);
	Mode_Init();
	USART_DMA_Config();
}

/****************************************************************************************
程序功能：USART的配置
程序版本：V1.0
日    期：2019/6/18
作    者：Orange
修    改：无
****************************************************************************************/
  
void usart_init(u32 bound)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  	//USART1_RX	  GPIOA.10初始化
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  	//Usart1 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

 	USART_Init(USART1, &USART_InitStructure);      //初始化串口1
  	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); //若总线空闲，产生中断 
  	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);   //开启串口DMA接收
	
  	USART_Cmd(USART1, ENABLE);                     //使能串口1 
}

/*****************************************************************************
程序功能：433M模式初始化配置
程序版本：V1.0
入口参数：无
返回参数：无
日    期：2019/6/18
作    者：Orange
修    改：无
******************************************************************************/

void Mode_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	                           //使能PB端口时钟
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		                             //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		                             //IO口速度为50MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;            //PB端口配置，MD0、MD1、AUX
	GPIO_Init(GPIOB, &GPIO_InitStructure);					                                 //根据设定参数初始化GPIOB
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11);
}

/*******************************************************************
程序功能：USART_DMA配置
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

void USART_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
	DMA_DeInit(DMA1_Channel5);   //将DMA的通道1寄存器重设为缺省值

	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1 -> DR));  //DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(RX_Data[0]);  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设到内存
	DMA_InitStructure.DMA_BufferSize = RX_SIZE;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;      //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //DMA通道 x拥最高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);  //
	
	DMA_Cmd(DMA1_Channel5, ENABLE);
}

/*****************************************************************************
程序功能：USART1发送数据
程序版本：V1.0
入口参数：TxBuffer：发送数据的首地址，Length：数据的长度
返回参数：无
日    期：2019/6/18
作    者：Orange
修    改：无
******************************************************************************/

void uart_tx_bytes( u8* TxBuffer, u8 Length)
{
	while(Length--)
	{
		while(RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		USART1->DR = *TxBuffer++;
	}
	
	State_SW = (State_SW&0x00) | 0x01;
}

/*******************************************************************
程序功能：DMA_ENABLE配置
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

void DMAEN_Config(void)
{
	if((1 == State_SW) && (1 == RX_Flg))
	{ 
		DMA_Cmd(DMA1_Channel5, ENABLE);  //开启DMA
	}
	else
	{
		//do nothing
	}
}

/*******************************************************************
程序功能：USART1_ISR配置
程序版本：V1.0
日    期： 2019/5/30 
作    者：Orange
修    改：无
*******************************************************************/

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)
    {
       	Data_Len = USART1 -> SR;
       	Data_Len = USART1-> DR; //清USART_IT_IDLE标志
	DMA_Cmd(DMA1_Channel5, DISABLE);    //关闭DMA
	Data_Len = RX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);      //得到真正接收数据个数  
	DMA1_Channel5->CNDTR = RX_SIZE;                        //重新设置接收数据个数  
	
       RX_Flg = 1;                      //接收数据标志位置1
    }
}



