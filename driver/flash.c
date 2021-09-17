#include "flash.h"
#include "string.h"
#include <stdlib.h>

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else
#define STM_SECTOR_SIZE  2048
#endif 

uint8_t STMFLASH_BUF[STM_SECTOR_SIZE];//����� 2K �ֽ�
//uint16_t WRITE_BUF[STM_SECTOR_SIZE/2];//����� 2K �ֽ�



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
       addr = WriteAddr-1;  //���Ǵ�ż��ַд��
	   ptr = (uint16_t*)(STMFLASH_BUF+offset-1);
	   lenNew = lenNew+1;
   }else{
	   addr = WriteAddr;
       ptr = (uint16_t*)(STMFLASH_BUF+offset);
   }
  
   for(i=0;i<lenNew;i++){
   	   FLASH_ProgramHalfWord(addr,ptr[i]);
	   addr+=2;//��ַ���� 2.
   }
}


uint32_t flash_read(uint32_t addr,uint8_t * data,uint32_t len)
{
    memcpy(data,(void*)addr,len);
	return len;
}

uint32_t flash_write(uint32_t addr,uint8_t * data,uint32_t len)
{
    uint32_t secpos;  //������ַ
    uint16_t secoff;  //������ƫ�Ƶ�ַ(16 λ�ּ���)
    uint16_t secremain; //������ʣ���ַ(16 λ�ּ���)
    uint16_t i;
	uint32_t write_len = 0;
	uint32_t alllen = len;
   
	if(addr<STM32_FLASH_BASE||((addr+len)>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE))){
		DEBUG("error address\r\n");
		return 0;//�Ƿ���ַ
	}

	FLASH_Unlock(); //����

	secpos = addr/STM_SECTOR_SIZE;   //�ڼ���������ʼ
	secoff = (addr%STM_SECTOR_SIZE); //�������ڵ�ƫ��
	secremain = STM_SECTOR_SIZE - secoff; //����ʣ��ռ��С


	if(len <= secremain){
		secremain=len;//�����ڸ�������Χ
	}

	while(1){
		flash_read(secpos*STM_SECTOR_SIZE,STMFLASH_BUF,STM_SECTOR_SIZE); //������������������
        //У������
		for(i=0;i<secremain;i++){
			if(STMFLASH_BUF[secoff+i]!=0XFF){
				break;//��Ҫ����
			}
		}
        //��Ҫ����
		if(i<secremain){
			//�����������
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE);
			//����
			for(i=0;i<secremain;i++){
				STMFLASH_BUF[i+secoff]=data[write_len];
				write_len++;
			}

			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE,0,STM_SECTOR_SIZE);//д����������
		}else{
		  
		   memcpy((STMFLASH_BUF+secoff),(data+write_len),secremain);
		
		   STMFLASH_Write_NoCheck(addr+write_len,secoff,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
		   write_len = write_len+ secremain; 
		}

        if(alllen <= write_len){
			break;//д�������
        }else{
            //д��δ����
            secpos++;  //������ַ�� 1
            secoff=0; //ƫ��λ��Ϊ 0
            len = alllen - write_len;  //�ֽ�(16 λ)���ݼ�
            //��һ����������д����
            if(len > STM_SECTOR_SIZE){
				secremain=STM_SECTOR_SIZE;
            }else{
                secremain=len;//��һ����������д����
            } 
		}
	}

	FLASH_Lock();//����
	
    return write_len;

}

