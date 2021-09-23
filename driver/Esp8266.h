#ifndef _ESP8266_H
#define _ESP8266_H


#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "systick.h"
#include <stdio.h>
#include <stdbool.h>
#include "data_struct.h"



#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define UDP 1


/******************************* ESP8266 �������Ͷ��� ***************************/
typedef enum{
	STA,
    AP,
    STA_AP,
    AUTOCONF	
} ENUM_Net_ModeTypeDef;


typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;
	

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;



typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;


typedef struct{
	uint8_t mode;  //���繤��ģʽ
	uint8_t ucTcpClosedFlag; //TPC��·�رձ�־
	uint8_t NetState;   //����״̬
	uint8_t change_ap_result;  //�任AP���
	uint8_t hb;                //������¼
	uint8_t hb_prb;            //��������
	uint8_t SN[SN_LEN];        //sn
	uint16_t reqVer;           //��������汾
	uint8_t upgradeRest;       //�������
	struct {
			__IO u16 join_ap:1;   //����AP״̬ bit 0
			__IO u16 link:1;      // ���ӷ�����״̬ bit1 
			__IO u16 change_ap:1; //�л�AP��־ bit2
			__IO u16 Penetrate:1; //͸����־
			__IO u16 getsn:1;     //��ȡ��SN
		    __IO u16 isUpgrade:1; //������־
		    __IO u16 upgrading:1; //��ʼ����
		    __IO u16 getip:1; //��ʼ����
	} State;

	struct{
      char ssid [AP_SSID_LEN];
	  char pass [AP_PASS_LEN];
	}AP_info;
	
	char ssid_tmp [AP_SSID_LEN];
	char pass_tmp [AP_PASS_LEN];
	char server_ip [IPLEN];
	char udps_ip [IPLEN];
	char sta_ip [IPLEN];
} ST_ESP_STATE;



/******************************* ESP8266 �ⲿȫ�ֱ������� ***************************/
#define RX_BUF_MAX_LEN     2050                                     //�����ջ����ֽ���

//��������֡�Ĵ���ṹ��

typedef struct{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
	union {
		__IO u16 InfAll;
		struct {
			__IO u16 FramLength       :15;                               // 14:0 
			__IO u16 FramFinishFlag   :1;                                // 15 
		} InfBit;
	}; 
}ST_Esp8266_Fram_Record;

#define            ESP8266_TcpServer_OverTime   "1800"             //��������ʱʱ�䣨��λ���룩
#define            NVIC_PriorityGroup_x                     NVIC_PriorityGroup_2

#define            ESP8266_TcpServer_IP         "dtu.heclouds.com" //"192.168.0.102"//
#define            ESP8266_TcpServer_Port       "8088"
#define            ESP8266_UdpServer_IP         "192.168.0.102" //"192.168.0.102"//
#define            ESP8266_UdpServer_Port       "8087"


/******************************** ESP8266 �������Ŷ��� ***********************************/
#define      ESP8266_USART_BAUD_RATE                       115200

#define      ESP8266_USARTx                                USART1
#define      ESP8266_USART_CLK                             RCC_APB2Periph_USART1
#define      ESP8266_USART_IRQ                             USART1_IRQn
#define      ESP8266_USART_INT_FUN                         USART1_IRQHandler



/*********************************************** ESP8266 �����궨�� *******************************************/
void ESP8266_Usart(char* data,uint16_t len);           
    





/****************************************** ESP8266 �������� ***********************************************/
void ESP8266_Init    (void);
void ESP8266_Rst(void);
void ESP8266_Rstore( void );
void ESP8266_Cmd(char * cmd);
void ESP8266_Netstate_set(uint8_t state);
bool ESP8266_Net_Mode_Choose( ENUM_Net_ModeTypeDef enumMode );
void ESP8266_JoinAP     ( char * pSSID, char * pPassWord );
void ESP8266_JoinAPCUR ( char * pSSID, char * pPassWord );
void ESP8266_JoinState(void);
void ESP8266_BuildAP( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode );
bool ESP8266_Enable_MultipleId         ( FunctionalState enumEnUnvarnishTx );
void ESP8266_Link_Server( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
void ESP8266_CLOSE_Link(void);
void SaveServerIP(ST_ESP_STATE *uc);

bool ESP8266_StartOrShutServer( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
void ESP8266_UnvarnishSend( void );
void ESP8266_ExitUnvarnishSend( void );
bool ESP8266_SendString( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId );
void ESP8266_process(void);
void IPD_process(char * data,uint16_t alllen,ST_ESP_STATE* uc);
void ESP8266_AT_send(ST_ESP_STATE *uc);
void ESP8266_AT_rsp(void);
void autoconfig(void);


#endif


