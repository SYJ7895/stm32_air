#ifndef __IMAGE__H__
#define __IMAGE__H__

#include "ctrlbsp.h"



#define UP_DEBUG 

#ifdef UP_DEBUG

	#define up_debug(fmt,args...)  DEBUG(fmt, ##args)
	#define up_err(fmt,args...)    DEBUG("upgrade:<%s> Fun:[%s] Line:%d \r\n"fmt,__func__,__LINE__, ##args)
#else
	#define up_debug(fmt,args...) 
	#define up_err(fmt,args...) 
#endif


#define IH_MAGIC_MCU            0x4B453036/* Image Magic Number for mcu system*/
#define IH_MAGIC_MCU_CFG        0x53595343/* Image Magic Number for mcu cfg*/
#define IH_NMLEN                32

typedef enum{
	ERR_MEM    = -2,
	ERR_TYPE   = -1,
	SOFT_TYPE  = 0,
	BOOT_IMAGE = 1,
	MAIN_MUC   = 2,
}IMAGE_TYPE;


typedef struct image_header {
    unsigned int    ih_magic;    /* Image Header Magic Number    */
    unsigned int    ih_hcrc;    /* Image Header CRC Checksum    */
    unsigned int    ih_hdmask;    /* hwcode    */
    unsigned int    ih_size;    /* Image Data Size        */
    unsigned int    ih_load;    /* Data     Load  Address        */
    unsigned int    ih_ep;        /* Entry Point Address        */
    unsigned int    ih_dcrc;    /* Image Data CRC Checksum    */
    unsigned char   ih_os;        /* Operating System        */
    unsigned char   ih_arch;    /* CPU architecture        */
    unsigned char   ih_type;    /* Image Type            */
    unsigned char   ih_comp;    /* Compression Type        */
    unsigned char   ih_name[IH_NMLEN];    /* Image Name        */
} __attribute__((packed))  image_header_t;


#endif
