#include "stm32f10x.h"
#include "ctrlbsp.h"
#include "HT1621.h"
#include "Sact-misc.h"
#include "Esp8266.h"
#include "i2c.h"
#include "flash.h"
#include "defcfg.h"
#include "wdog.h"

struct st{
	uint8_t id;
	uint8_t data;	
};

uint8_t gIsConf = 0;
uint8_t gNetState = 0;

int main()
{
	uint8_t start = 1;
	CTRLBSP_Init();
	//26sÎ¹¹·Ò»´Î
//	IWDG_Init(6,4095);
	
	flash_read(ISCONFIG, (uint8_t*)&gIsConf, sizeof(gIsConf));
	
	if(gIsConf != 1){
	   BL_SetHigh;
	}
	Ht1621_Init();
	LCD_Disp_Init();
	DEBUG("test hello soft ver %d\r\n",SOFT_VERSION);
	
	ESP8266_Init();
	
	while(start){
	#if 1
		ESP8266_process();
		LCD_Disp_Update();
	#endif
		
		IWDG_Feed();
	}
	return 0;
}
