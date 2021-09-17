#ifndef WDOG_H
#define WDOG_H

#include "ctrlbsp.h"

void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);


#endif
