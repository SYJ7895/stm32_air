#include "wdog.h"
#include "stm32f10x_iwdg.h"

//初始化独立看门狗
//prer:分频数:0~7(只有低 3 位有效!)
//分频因子=4*2^prer.但最大值只能是 256!
//rlr:重装载寄存器值:低 11 位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
void IWDG_Init(u8 prer,u16 rlr)
{ 
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //①使能对寄存器 I 写操作
    IWDG_SetPrescaler(prer); //②设置 IWDG 预分频值:设置 IWDG 预分频值
    IWDG_SetReload(rlr); //②设置 IWDG 重装载值
    IWDG_ReloadCounter(); //③按照 IWDG 重装载寄存器的值重装载 IWDG 计数器
    IWDG_Enable(); //④使能 IWDG
}
//喂独立看门狗
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();//reload
}
