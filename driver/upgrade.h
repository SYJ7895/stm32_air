#ifndef   __UPGRADE_H__
#define   __UPGRADE_H__

#include "stdint.h"
#include "data_struct.h"

/**
 * 数据包序号类型枚举定义 
 */
typedef enum{
	SEQHEAD = 0, /**< 首个升级包  */
	SEQNOEXIST = 0xfe, /**<不存在的升级包 */
	SEQEND  = 0xff, /**< 最后一个升级包  */
}SEQNUM;

/**
 * 升级返回值类型枚举定义 
 */
typedef enum{
	DATA_ERR = -1, /**< 首个升级包  */
	DATA_OK  = 0, /**< 首个升级包  */
}UP_RET;


int upgrade(void *data,uint16_t len, SEQNUM seqnum);

#endif

