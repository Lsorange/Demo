#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define N_KEY            0        //�ް�������
#define S_KEY            1        //�а���������Ϊ�̰�
#define L_KEY            2        //�а���������Ϊ����

#define N_KEY_State      0        //�ް���
#define S_KEY_State      1        //�̰������ֹͣ
#define L_KEY_State      2        //�����������������
#define KDelay_Time      200    //3-4s����������ʵ�ʵ����ȷ��

//#define Motor_Stop_KEY  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)

//������
#define Motor_Stop_KEY  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)


void MKEY_CallBack(void);
void MKEY_Init(void);
//void Stop_KEY_Init(void);
//u8 Get_KEY_State(u16 time);
u8 KEY_Scan(void);

		
#endif
