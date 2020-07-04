#ifndef __WDG_H
#define __WDG_H
#include "sys.h"

#define PRER 4
#define RLR  6250

void MWDG_CallBack(void);
void MWDG_Init(void);
//void IWDG_Init(u8 prer,u16 rlr);

#endif
