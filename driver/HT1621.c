#include "HT1621.h"



 #define _Nop() __nop()
 #define BIAS     0x52             //0b1000 0101 0010  1/3duty 4com
 #define SYSDIS   0X00           //0b1000 0000 0000  ����ϵͳ������LCDƫѹ������
 #define LCDOFF   0X04   //0b1000 0000 0100  ��LCDƫѹ
 #define LCDON    0X06   //0b1000 0000 0110  ��LCDƫѹ
 #define SYSEN    0X02   //0b1000 0000 0010 ��ϵͳ����
 #define XTAL     0x28   //0b1000 0010 1000 �ⲿ��ʱ��
 #define RC256    0X30   //0b1000 0011 0000  �ڲ�ʱ��
 #define TONEON   0X12   //0b1000 0001 0010  ���������
 #define TONEOFF  0X10   //0b1000 0001 0000 �ر��������
 #define WDTDIS   0X0A   //0b1000 0000 1010  ��ֹ���Ź�
 
  



 //���дһ���������ͷ(100)
 void Ht1621WrCmd(uchar ucTmpCmd)
 {
	 uchar i,ucCmdByte=0x80;
	 //����CS������д��
	 HT1621_CS_LOW();
	 
	 delay_us(HT1621_Delay);
	 //����д����ģʽ
	 for (i=0;i<4;i++){
		 //�����ݷ�������
		 if (ucCmdByte&0x80){
			 HT1621_DAT_HIGH();
		 }else{
			 HT1621_DAT_LOW();
		 }
		 //����д����
		 HT1621_WR_LOW();
		 delay_us(HT1621_Delay);
		 HT1621_WR_HIGH();
		 delay_us(HT1621_Delay);
		 ucCmdByte<<=1;
	 }
	 //д������
	 for (i=0;i<8;i++){
		 if (ucTmpCmd&0x80){
			 HT1621_DAT_HIGH();
		 }else{
			 HT1621_DAT_LOW();
		 }

		 HT1621_WR_LOW();
		 delay_us(HT1621_Delay);
		 HT1621_WR_HIGH();
		 delay_us(HT1621_Delay);
		 ucTmpCmd<<=1;
	 }
     //����CS������д��
	 HT1621_CS_HIGH();
	 delay_us(HT1621_Delay);
	 
 }
 
 
 //д����+��ַ
 void Ht1621WrDataCmd(uchar ucAddr)
 {
	 //101  д����ģʽ
	 uchar i,ucDataCmd=0x05;    
	 ucDataCmd<<=5;
	 
	 
	 for (i=0;i<3;i++){
		  //�����ݷ�������
		 if (ucDataCmd&0x80){
			 HT1621_DAT_HIGH();
		 }else{
			 HT1621_DAT_LOW();
		 }
		  //����д����
		 HT1621_WR_LOW();
		 delay_us(HT1621_Delay);
		 HT1621_WR_HIGH();
		 delay_us(HT1621_Delay);
		 ucDataCmd<<=1; 
	 }

	 ucAddr<<=2;
	 for (i=0;i<6;i++){
		 if (ucAddr&0x80){
			 HT1621_DAT_HIGH();
		 }else{
			 HT1621_DAT_LOW();
		 }
		 
		 HT1621_WR_LOW();
		 delay_us(HT1621_Delay);
		 HT1621_WR_HIGH();
		 delay_us(HT1621_Delay);
		 ucAddr<<=1;
	 }
 }
 
 //���дһ���ֽ�����
 void Ht1621WrByte(uchar ucTmpData)
 {
	 uchar i;

	 for (i=0;i<4;i++){
		 if (ucTmpData&0x08){
			 HT1621_DAT_HIGH();
		 }else{
			 HT1621_DAT_LOW();
		 }

		 HT1621_WR_LOW();
		 delay_us(HT1621_Delay);
		 HT1621_WR_HIGH();
		 delay_us(HT1621_Delay); 
		 ucTmpData<<=1;
	 }
 }

 void Ht1621_Init()
 {
	 int i=0;
  
	 HT1621_CS_HIGH();
	 HT1621_WR_LOW();
	 HT1621_DAT_HIGH();

	  //��ʱʹLCD������ѹ�ȶ�
	 delay_ms(200); 
	 Ht1621WrCmd(BIAS);
	 
	 //ʹ���ڲ�����
	 Ht1621WrCmd(RC256); 
	 Ht1621WrCmd(SYSDIS);
	 Ht1621WrCmd(WDTDIS);
	 Ht1621WrCmd(SYSEN);
	 
	 for (i=0;i<10;i++){
		 HT1621_CS_LOW();
         //��RAM�ĵ�ַ4д����  ����Ĳ�����ΧΪ:0-31���õ��ľ�0-9,0-3Ϊ�����¶�����4-7Ϊʵ���¶ȣ�8-9Ϊͼ�ο�������
		 Ht1621WrDataCmd(i);  
		 Ht1621WrByte(0x0);
		 HT1621_CS_HIGH();
		 delay_us(HT1621_Delay); 
	 } 
	 
	 Ht1621WrCmd(LCDON);
 }
 

