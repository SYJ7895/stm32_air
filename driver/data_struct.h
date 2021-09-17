#ifndef __DATA_STRUCT_H
#define __DATA_STRUCT_H

#include "stm32f10x.h"
#include "misc.h"
#include "systick.h"

#define ONE_NET

#define AP_SSID_LEN  20
#define AP_PASS_LEN  20
#define IPLEN        20
#define HB_LOSE      3  //3����û���յ�����
#define SN_LEN       16


typedef enum{
/****************������********************/
    IOT_REMOUNT          = 0x00,  //�ն��ع���
    IOT_MOUNT_REQ        = 0x01,  //�ն˹�������
    IOT_MOUNT_RSP        = 0x02,  //������Ӧ
    IOT_HB_REQ           = 0x03,  //����������
    IOT_HB_RSP           = 0x04,  //�������ظ�
    IOT_UPGRADE_NOTE     = 0x05,  //����֪ͨ
    IOT_UPGRADE_REQ      = 0x06,  //��������
    IOT_UPGRADE_RSP      = 0x07,  //����������Ӧ
    IOT_UPGRADE_RESULT   = 0x08,  //��������ϱ�
	JOIN_AP_REQ          = 0x09,  //����AP
	JOIN_AP_RSP          = 0x0A,  //����AP��Ӧ
	CHANGE_AP_REQ        = 0x0B,  //�л�AP֪ͨ
	CHANGE_AP_RSP        = 0x0C,  //�л�AP��Ӧ

/******************ҵ����*********************/

    IOT_REPORT_REQ       = 0x20,  //�¶ȡ����ٵ��ϱ�
    IOT_REPORT_RSP       = 0x21,  //�ϱ���Ӧ
    IOT_SET_REQ          = 0x22,  //�����¶ȡ����ٵ�����
    IOT_SET_RSP          = 0x23,  //������Ӧ
    IOT_LOG_REPORT       = 0x24,  //��־�ϱ�
    IOT_LOG_REPORT_RSP   = 0x25,  //��־�ϱ���Ӧ
    IOT_GET_REQ          = 0x26,  //������ȡ��������
    IOT_GET_RSP          = 0x27,  //������ȡ������Ӧ
    IOT_GET_LOG_REQ      = 0x28,  //������ȡLOG����
    IOT_GET_LOG_RSP      = 0x29,  //������ȡLOG��Ӧ
 
} MSG_EVENT_ID;


typedef enum{
	NET_LOSE = 0, //����Ͽ�
	NET_MOUNT,    //����
	NET_MOUNTING, //���ڹ���
	NET_NORMAL,   //��������
	UPGRADE,      //������
}ENUM_NET_STATE;


typedef enum{
	SET_TMPE = 0,  //�����¶�
	SET_MODE,      //����ģʽ
	SET_SPEED,     //���÷���
	SET_SWITCH,    //���ÿ���
}ENUM_SET_MODE;

typedef enum{
	NOUPGRADE = 0,  //û������
	UPGRADEOK ,      //�����ɹ�
	DOWNLOADOK ,  //���سɹ�
	FILENOEXIST,  //�ļ�������
	UPGRADEFAIL,     //����ʧ��
}ENUM_UPGRADE_RESULT;


typedef enum{
	SET_TMPE_DO   = 0,  //�����¶�
	SET_MODE_COOL,      //��������ģʽ
	SET_MODE_WARM,      //��������ģʽ
	SET_MODE_WIND,      //�����ͷ�ģʽ
	SET_SPEED_AUTO,     //���÷����Զ�
	SET_SPEED_LOW,      //���÷��ٵ�
	SET_SPEED_MID,      //���÷�����
	SET_SPEED_HIGH,     //���÷��ٸ�
	SET_SWITCH_ON,      //�򿪿յ�
	SET_SWITCH_OFF,     //�رտյ�
}ENUM_SET_VULE;

typedef enum{
	GET_REAL_TMPE = 0,  //��ȡ��ǰʵ���¶�
	GET_SET_TMPE,       //��ȡ�����¶�
	GET_MODE,           //��ȡģʽ
	GET_SPEED,          //��ȡ����
	GET_SWITCH,         //��ȡ����״̬
}ENUM_GET_MODE;




/*��ͷ��Ϣ*/
typedef struct {
    uint8_t  crc8;         //crc
    uint16_t len;          //���ݳ���
    uint8_t event_id;      //�¼�ID
    uint8_t seq;          //����seq
} __attribute__((packed)) ST_PACK_INFO;

/*��������*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint16_t soft_ver;  //����汾
    uint16_t cfg_ver;   //���ð汾
	uint8_t upgradeR;   //�������
} __attribute__((packed)) ST_MOUNT_REQ;

/*������Ӧ*/
typedef struct {
    uint32_t timetick;   //ϵͳ��ʱ���
    uint8_t  hb_per;     //��������
} __attribute__((packed)) ST_MOUNT_RSP;


/*�����ϱ�*/
typedef struct {
    uint8_t SN[SN_LEN];
} __attribute__((packed)) ST_HB_REQ;


/*����֪ͨ*/
typedef struct {
    uint16_t softVer;   //����汾�� 
} __attribute__((packed)) ST_UPGRADE_NOTICE;


/*��������*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint16_t softVer;   //����汾�� 
    uint8_t seqnum;   //�����
} __attribute__((packed)) ST_UPGRADE_REQ;

/*�������֪ͨ*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t result;   //�������
} __attribute__((packed)) ST_UPGRADE_INFO;





/*����AP ��Ϣ*/
typedef struct {
    char ssid[AP_SSID_LEN]; //AP SSID
    char pass[AP_PASS_LEN]; //AP pass
} __attribute__((packed)) ST_AP_INFO;

/*join ap rsp*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t result;  //join ap ���
    uint8_t reason;  //ʧ��ԭ��
} __attribute__((packed)) ST_JOIN_AP_RSP;


/*�ϱ���Ϣ*/
typedef struct {
	uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t real_tmp;  //��ǰ����
    uint8_t set_tmp;   //���õ��¶�
    uint8_t  mode;     //��ǰģʽ
    uint8_t  spped;    //��ǰ����
    uint8_t  on_off;   //��ǰ����״̬
} __attribute__((packed)) ST_REPORT_INFO;

/*Զ��������Ϣ*/
typedef struct {
    uint8_t  mode;    //�����¼�����
    uint8_t vaule;    //���õ�ֵ
} __attribute__((packed)) ST_SET_INFO;

/*Զ�����ý���ϱ�*/
typedef struct {
	uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t  mode;    //�����¼�����
    uint8_t result;   //���ý��
} __attribute__((packed)) ST_SET_RSP;


/*Զ�̻�ȡ����*/
typedef struct {
     uint8_t  mode;    //��ȡ����
} __attribute__((packed)) ST_GET_REQ;

/*��ȡ��Ӧ*/
typedef struct {
    uint8_t SN[SN_LEN]; // SN 16byte
    uint8_t  mode;      //����
    uint8_t value;      //��ֵ
} __attribute__((packed)) ST_GET_RSP;


/*��־�ϱ�*/
typedef struct {
    uint8_t   SN[SN_LEN]; // SN 16byte
    uint16_t  len;      //����
    char      data[1024];     //LOG
} __attribute__((packed)) ST_LOG_REPORT;











#endif

