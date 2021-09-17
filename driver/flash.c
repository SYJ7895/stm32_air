#include "flash.h"
#include "string.h"
#include <stdlib.h>

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else
#define STM_SECTOR_SIZE  2048
#endif 

uint8_t STMFLASH_BUF[STM_SECTOR_SIZE];//最多是 2K 字节
//uint16_t WRITE_BUF[STM_SECTOR_SIZE/2];//最多是 2K 字节



void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t offset,uint16_t NumToWrite)
{

   uint16_t lenNew =  0;
   uint16_t i  =0;
   uint16_t * ptr = NULL;
   uint32_t addr ;
   
	
//   memset(WRITE_BUF,0xFF,STM_SECTOR_SIZE);
    
   if(NumToWrite%2){
	   lenNew = (NumToWrite+1)/2;	   
   }else{
	   lenNew = NumToWrite/2;
   }
   
   if(offset%2){
       addr = WriteAddr-1;  //都是从偶地址写入
	   ptr = (uint16_t*)(STMFLASH_BUF+offset-1);
	   lenNew = lenNew+1;
   }else{
	   addr = WriteAddr;
       ptr = (uint16_t*)(STMFLASH_BUF+offset);
   }
  
   for(i=0;i<lenNew;i++){
   	   FLASH_ProgramHalfWord(addr,ptr[i]);
	   addr+=2;//地址增加 2.
   }
}


uint32_t flash_read(uint32_t addr,uint8_t * data,uint32_t len)
{
    memcpy(data,(void*)addr,len);
	return len;
}

uint32_t flash_write(uint32_t addr,uint8_t * data,uint32_t len)
{
    uint32_t secpos;  //扇区地址
    uint16_t secoff;  //扇区内偏移地址(16 位字计算)
    uint16_t secremain; //扇区内剩余地址(16 位字计算)
    uint16_t i;
	uint32_t write_len = 0;
	uint32_t alllen = len;
   
	if(addr<STM32_FLASH_BASE||((addr+len)>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE))){
		DEBUG("error address\r\n");
		return 0;//非法地址
	}

	FLASH_Unlock(); //解锁

	secpos = addr/STM_SECTOR_SIZE;   //第几个扇区开始
	secoff = (addr%STM_SECTOR_SIZE); //在扇区内的偏移
	secremain = STM_SECTOR_SIZE - secoff; //扇区剩余空间大小


	if(len <= secremain){
		secremain=len;//不大于该扇区范围
	}

	while(1){
		flash_read(secpos*STM_SECTOR_SIZE,STMFLASH_BUF,STM_SECTOR_SIZE); //读出整个扇区的内容
        //校验数据
		for(i=0;i<secremain;i++){
			if(STMFLASH_BUF[secoff+i]!=0XFF){
				break;//需要擦除
			}
		}
        //需要擦除
		if(i<secremain){
			//擦除这个扇区
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE);
			//复制
			for(i=0;i<secremain;i++){
				STMFLASH_BUF[i+secoff]=data[write_len];
				write_len++;
			}

			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE,0,STM_SECTOR_SIZE);//写入整个扇区
		}else{
		  
		   memcpy((STMFLASH_BUF+secoff),(data+write_len),secremain);
		
		   STMFLASH_Write_NoCheck(addr+write_len,secoff,secremain);//写已经擦除了的,直接写入扇区剩余区间.
		   write_len = write_len+ secremain; 
		}

        if(alllen <= write_len){
			break;//写入结束了
        }else{
            //写入未结束
            secpos++;  //扇区地址增 1
            secoff=0; //偏移位置为 0
            len = alllen - write_len;  //字节(16 位)数递减
            //下一个扇区还是写不完
            if(len > STM_SECTOR_SIZE){
				secremain=STM_SECTOR_SIZE;
            }else{
                secremain=len;//下一个扇区可以写完了
            } 
		}
	}

	FLASH_Lock();//上锁
	
    return write_len;

}

