#include "upgrade.h"
#include "image.h"
#include "cpu_trans.h"
#include "image_update.h"
#include "flash.h"
#include "stdlib.h"


#define section_size   1024

uint8_t datatmp[section_size] = {0};



/*
*  void *data  : 数据指针
*  uint32_t len     ：数据长度
*  uint32_t seqnum   ：数据包序列
*/
int upgrade(void *data,uint16_t len, uint8_t seqnum)
{
	static int8_t type = ERR_TYPE;
	static uint32_t write_len = 0;
	static uint16_t write_now = 0;
	static uint32_t data_len  = 0;
	uint8_t upinfo = 0;
	static uint32_t addr = 0;
	static uint8_t first_data = 0;
	static uint8_t seq = 0;
	
	int16_t ret = DATA_ERR;
	IMAGE_TYPE checkret = ERR_TYPE;
	
	if(seq == seqnum  || seqnum == SEQEND){
       seq ++;
	}else{
	   seq = 0;
       up_debug(" data seq error! \r\n");
	   goto OUT;
	}
	
	if( seqnum == SEQHEAD ){         //第一帧头部进行头部校验
		seq = 1;
		type = ERR_TYPE;
		write_len = 0;
		write_now = 0;
		data_len = 0;
		first_data = 1;		
	}
	
	if(first_data){
		memcpy((datatmp+write_now),data,len);
		write_now += len;
		if(write_now >= sizeof(image_header_t)){
			image_header_t head ;
			memcpy((unsigned char*)&head,datatmp,sizeof(image_header_t));
			checkret = sys_check_image_header((unsigned char *)&head,sizeof(image_header_t));
			data_len = mcu_ntohl(head.ih_size);
			first_data = 0;
			if(checkret != SOFT_TYPE){
				up_debug(" head data type error! \r\n");
				goto OUT;
			}
			
	
			if((data_len+IMAGE_HEAD_SIZE) > SOFT_SIZE ){
				ret = DATA_ERR;
				up_debug("data len too long \r\n");
				goto OUT; 
			}
			addr = SOFT_TMP_ADDR;
				
		}
	}else{
		if( (write_now+len) < section_size){
			memcpy((datatmp+write_now),data,len);
			write_now += len;
		}else{
			if(write_now != section_size)
				memcpy((datatmp+write_now),data,(section_size-write_now));
			ret = flash_write( addr+write_len ,datatmp,section_size);
					
			if(ret != section_size){
				up_debug("package type %d upgrade write error ret = %d \r\n",type,ret);
				ret = DATA_ERR;
				goto OUT;
			}		
			write_len = write_len + section_size;
			write_now = len - (section_size -write_now);
			memset(datatmp,0,section_size);
			if(write_now)
				memcpy(datatmp,((char*)data+(len -write_now)),write_now);
			
		}
		
		
		if( seqnum == SEQEND ){    //数据接收结束，做crc校验
			if(write_now && write_len != (data_len+IMAGE_HEAD_SIZE) ){
				ret = flash_write( addr+write_len ,datatmp,(data_len + IMAGE_HEAD_SIZE - write_len));
				if(ret != (data_len + IMAGE_HEAD_SIZE - write_len)){
				    up_debug("2 package type %d upgrade write error ret = %d \r\n",type,ret);
				    ret = DATA_ERR;
				    goto OUT;
			  }		
			}
			
			if( sys_check_mcu_image(SOFT_TMP_ADDR) != DATA_OK){
					up_debug("boot data check crc error ! \r\n");
					ret = DATA_ERR;
				    goto OUT;
			}
  
			upinfo = DOWNLOADOK;
			ret = flash_write(UPGRADE_INFO_ADDR,(uint8_t*)&upinfo,sizeof(upinfo));
			if(ret != 1){
				up_debug("upgrade write upinfo error at %x \r\n",UPGRADE_INFO_ADDR);
				ret = DATA_ERR;
				goto OUT;
			}
			
		}
	 }

	return DATA_OK;
OUT:
	return ret;
}


