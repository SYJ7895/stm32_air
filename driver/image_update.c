#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "image.h"
#include "cpu_trans.h"
#include "crc32.h"
#include "upgrade.h"
#include "flash.h"

IMAGE_TYPE sys_check_image_header(unsigned char *image_header ,unsigned int len)
{
    unsigned int chksum = 0,cal_chksum = 0;
    image_header_t hdr;
    IMAGE_TYPE ret = ERR_TYPE;
    if(len != sizeof(image_header_t)||image_header == NULL){
        up_debug("param error!");
        return ret;
    }
    memcpy((unsigned char*)&hdr,image_header,sizeof(image_header_t));
    /* Check header magic number */
    if(mcu_ntohl(hdr.ih_magic) == IH_MAGIC_MCU){
	   ret = SOFT_TYPE;
    }else{
        up_debug("your magic is not IH_MAGIC_MCU \r\n");
        return ret;
    }
    up_debug("your magic is ok,ih_name = %s \r\n",hdr.ih_name);
    /* Check header crc */
    /* Skip crc checking if there is no valid header, or it may hang on due to broken header length */
    chksum = mcu_ntohl(hdr.ih_hcrc);
    hdr.ih_hcrc = 0;
    cal_chksum = mcu_crc32(0, (unsigned char*)&hdr, len);/*common crc*/
    if (cal_chksum != chksum) {
        up_debug("Image header checksum calc = %x ,read = %x\r\n",cal_chksum , chksum);
	    ret = ERR_TYPE;
    }
    else{
        up_debug("Image header check OK\r\n");
    }
    return ret;
}

int sys_check_mcu_image(uint32_t addr)
{
    int ret = -1;
    unsigned int len = 0, sec_len = 0, chksum = 0;
    image_header_t *hdr = NULL;
    unsigned char* hdr_addr = NULL;
    int e_size = 64;
    unsigned char buf[64] = {0};
    unsigned int offset = 0, calc_crc = 0 , blk_cnt = 0 , i = 0;
    
	up_debug("%d ",e_size);
    /*check header first*/
    ret = flash_read(addr,buf,e_size);
    if(ret != e_size){
        up_debug("read size = %d ,need = %d ,read error",ret,e_size);
        ret = DATA_ERR;
        goto ERROR_OUT;
    }

    /* Check header magic number */
    if(sys_check_image_header((unsigned char*)buf,sizeof(image_header_t)) == ERR_TYPE){
        up_debug("img check error\r\n");
        ret = DATA_ERR;
        goto ERROR_OUT;
    }
    hdr = (image_header_t*)buf;
    len = (unsigned int)mcu_ntohl(hdr->ih_size);
    chksum = (unsigned int)mcu_ntohl(hdr->ih_dcrc);

    up_debug("image len =  %d\r\n",len);
    /*calc first block*/
    sec_len = e_size - sizeof(image_header_t);
    hdr_addr = (unsigned char*)(buf + sizeof(image_header_t));
    /*crc skip header*/
    if(sec_len){
        calc_crc = mcu_crc32(0, (unsigned char *)(hdr_addr), sec_len);
    }
    len -= sec_len;
    offset = e_size;
    blk_cnt = len / e_size;/*full block crc*/
    for(i = 0 ; i < blk_cnt ; i++){
        ret = flash_read(addr+offset,buf,e_size);
        if(ret != e_size){
            up_debug("read size = %d ,need = %d ,read error,i = %d,offset= %x\r\n",ret,e_size,i,offset);
            ret = DATA_ERR;
            goto ERROR_OUT;
        }
        offset += e_size;
        calc_crc = mcu_crc32(calc_crc, (unsigned char *)(buf), e_size);
    }
		
    /*last block*/
    if(len % e_size != 0){
        ret = flash_read(addr+offset,buf,len % e_size);
        if(ret != (len % e_size)){
            up_debug("read size = %d ,need = %d ,read error\r\n",ret,len % e_size);
            ret = DATA_ERR;
            goto ERROR_OUT;
        }
        calc_crc = mcu_crc32(calc_crc, (unsigned char *)(buf),  len % e_size);
        up_debug("calc_crc = %x\r\n",calc_crc);
    }
    
    if(chksum != calc_crc){
        ret = DATA_ERR;
        up_debug("chksum = %x,calc = %x not equal\r\n",chksum,calc_crc);
        goto ERROR_OUT;
    }else{
       up_debug("chksum ok\r\n");
    }
    ret = DATA_OK;
ERROR_OUT:
    up_debug("leave ret =%d\r\n",ret);
    return ret;
}
