#ifndef   __IMAGE_UPDATE_H__
#define   __IMAGE_UPDATE_H__
#include "image.h"

IMAGE_TYPE sys_check_image_header(unsigned char *image_header ,unsigned int len);
int sys_check_mcu_image(uint32_t addr);




#endif

