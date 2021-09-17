#ifndef __DATA_STRUCT_H
#define __DATA_STRUCT_H

#include "stm32f10x.h"
#include "misc.h"
#include "systick.h"

#define ONE_NET

#define AP_SSID_LEN  20
#define AP_PASS_LEN  20
#define IPLEN        20
#define HB_LOSE      3  //3分钟没有收到心跳
#define SN_LEN       16


typedef enum{
/****************管理类********************/
    IOT_REMOUNT          = 0x00,  //终端重挂载
    IOT_MOUNT_REQ        = 0x01,  //终端挂载请求
    IOT_MOUNT_RSP        = 0x02,  //挂载响应
    IOT_HB_REQ           = 0x03,  //心跳包发送
    IOT_HB_RSP           = 0x04,  //心跳包回复
    IOT_UPGRADE_NOTE     = 0x05,  //升级通知
    IOT_UPGRADE_REQ      = 0x06,  //升级请求
    IOT_UPGRADE_RSP      = 0x07,  //升级请求响应
    IOT_UPGRADE_RESULT   = 0x08,  //升级结果上报
	JOIN_AP_REQ          = 0x09,  //加入AP
	JOIN_AP_RSP          = 0x0A,  //加入AP响应
	CHANGE_AP_REQ        = 0x0B,  //切换AP通知
	CHANGE_AP_RSP        = 0x0C,  //切换AP响应

/******************业务类*********************/

    IOT_REPORT_REQ       = 0x20,  //温度、风速等上报
    IOT_REPORT_RSP       = 0x21,  //上报响应
    IOT_SET_REQ          = 0x22,  //设置温度、风速等请求
    IOT_SET_RSP          = 0x23,  //设置响应
    IOT_LOG_REPORT       = 0x24,  //日志上报
    IOT_LOG_REPORT_RSP   = 0x25,  //日志上报响应
    IOT_GET_REQ          = 0x26,  //主动获取数据请求
    IOT_GET_RSP          = 0x27,  //主动获取数据响应
    IOT_GET_LOG_REQ      = 0x28,  //主动获取LOG请求
    IOT_GET_LOG_RSP      = 0x29,  //主动获取LOG响应
 
} MSG_EVENT_ID;


typedef enum{
	NET_LOSE = 0, //网络断开
	NET_MOUNT,    //挂载
	NET_MOUNTING, //正在挂载
	NET_NORMAL,   //网络正常
	UPGRADE,      //升级中
}ENUM_NET_STATE;


typedef enum{
	SET_TMPE = 0,  //设置温度
	SET_MODE,      //设置模式
	SET_SPEED,     //设置风速
	SET_SWITCH,    //设置开关
}ENUM_SET_MODE;

typedef enum{
	NOUPGRADE = 0,  //没有升级
	UPGRADEOK ,      //升级成功
	DOWNLOADOK ,  //下载成功
	FILENOEXIST,  //文件不存在
	UPGRADEFAIL,     //升级失败
}ENUM_UPGRADE_RESULT;


typedef enum{
	SET_TMPE_DO   = 0,  //设置温度
	SET_MODE_COOL,      //设置制冷模式
	SET_MODE_WARM,      //设置制热模式
	SET_MODE_WIND,      //设置送风模式
	SET_SPEED_AUTO,     //设置风速自动
	SET_SPEED_LOW,      //设置风速低
	SET_SPEED_MID,      //设置风速中
	SET_SPEED_HIGH,     //设置风速高
	SET_SWITCH_ON,      //打开空调
	SET_SWITCH_OFF,     //关闭空调
}ENUM_SET_VULE;

typedef enum{
	GET_REAL_TMPE = 0,  //获取当前实际温度
	GET_SET_TMPE,       //获取设置温度
	GET_MODE,           //获取模式
	GET_SPEED,          //获取风速
	GET_SWITCH,         //获取开关状态
}ENUM_GET_MODE;




/*包头信息*/
typedef struct {
    uint8_t  crc8;         //crc
    uint16_t len;          //数据长度
    uint8_t event_id;      //事件ID
    uint8_t seq;          //报文seq
} __attribute__((packed)) ST_PACK_INFO;

/*挂载请求*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint16_t soft_ver;  //软件版本
    uint16_t cfg_ver;   //配置版本
	uint8_t upgradeR;   //升级结果
} __attribute__((packed)) ST_MOUNT_REQ;

/*挂载响应*/
typedef struct {
    uint32_t timetick;   //系统秒时间戳
    uint8_t  hb_per;     //心跳周期
} __attribute__((packed)) ST_MOUNT_RSP;


/*心跳上报*/
typedef struct {
    uint8_t SN[SN_LEN];
} __attribute__((packed)) ST_HB_REQ;


/*升级通知*/
typedef struct {
    uint16_t softVer;   //软件版本号 
} __attribute__((packed)) ST_UPGRADE_NOTICE;


/*升级请求*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint16_t softVer;   //软件版本号 
    uint8_t seqnum;   //包序号
} __attribute__((packed)) ST_UPGRADE_REQ;

/*升级结果通知*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t result;   //升级结果
} __attribute__((packed)) ST_UPGRADE_INFO;





/*接收AP 信息*/
typedef struct {
    char ssid[AP_SSID_LEN]; //AP SSID
    char pass[AP_PASS_LEN]; //AP pass
} __attribute__((packed)) ST_AP_INFO;

/*join ap rsp*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t result;  //join ap 结果
    uint8_t reason;  //失败原因
} __attribute__((packed)) ST_JOIN_AP_RSP;


/*上报信息*/
typedef struct {
	uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t real_tmp;  //当前室温
    uint8_t set_tmp;   //设置的温度
    uint8_t  mode;     //当前模式
    uint8_t  spped;    //当前风速
    uint8_t  on_off;   //当前开关状态
} __attribute__((packed)) ST_REPORT_INFO;

/*远程设置信息*/
typedef struct {
    uint8_t  mode;    //设置事件类型
    uint8_t vaule;    //设置的值
} __attribute__((packed)) ST_SET_INFO;

/*远程设置结果上报*/
typedef struct {
	uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t  mode;    //设置事件类型
    uint8_t result;   //设置结果
} __attribute__((packed)) ST_SET_RSP;


/*远程获取请求*/
typedef struct {
     uint8_t  mode;    //获取类型
} __attribute__((packed)) ST_GET_REQ;

/*获取响应*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t  mode;      //类型
    uint8_t value;      //数值
} __attribute__((packed)) ST_GET_RSP;


/*日志上报*/
typedef struct {
    uint8_t   SN[SN_LEN]; // SN 16byte
    uint16_t  len;      //类型
    char      data[1024];     //LOG
} __attribute__((packed)) ST_LOG_REPORT;











#endif

