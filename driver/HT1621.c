#include "HT1621.h"



 #define _Nop() __nop()
 #define BIAS     0x52             //0b1000 0101 0010  1/3duty 4com
 #define SYSDIS   0X00           //0b1000 0000 0000  关振系统荡器和LCD偏压发生器
 #define LCDOFF   0X04   //0b1000 0000 0100  关LCD偏压
 #define LCDON    0X06   //0b1000 0000 0110  打开LCD偏压
 #define SYSEN    0X02   //0b1000 0000 0010 打开系统振荡器
 #define XTAL     0x28   //0b1000 0010 1000 外部接时钟
 #define RC256    0X30   //0b1000 0011 0000  内部时钟
 #define TONEON   0X12   //0b1000 0001 0010  打开声音输出
 #define TONEOFF  0X10   //0b1000 0001 0000 关闭声音输出
 #define WDTDIS   0X0A   //0b1000 0000 1010  禁止看门狗
 
  



 //完成写一个命令包括头(100)
 void Ht1621WrCmd(uchar ucTmpCmd)
 {
	 uchar i,ucCmdByte=0x80;
	 //拉低CS，允许写入
	 HT1621_CS_LOW();
	 
	 delay_us(HT1621_Delay);
	 //进入写命令模式
	 for (i=0;i<4;i++){
		 //将数据放入总线
		 if (ucCmdByte&0x80){
			 HT1621_DAT_HIGH();
		 }else{
			 HT1621_DAT_LOW();
		 }
		 //发送写脉冲
		 HT1621_WR_LOW();
		 delay_us(HT1621_Delay);
		 HT1621_WR_HIGH();
		 delay_us(HT1621_Delay);
		 ucCmdByte<<=1;
	 }
	 //写入命令
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
     //拉高CS不允许写入
	 HT1621_CS_HIGH();
	 delay_us(HT1621_Delay);
	 
 }
 
 
 //写命令+地址
 void Ht1621WrDataCmd(uchar ucAddr)
 {
	 //101  写数据模式
	 uchar i,ucDataCmd=0x05;    
	 ucDataCmd<<=5;
	 
	 
	 for (i=0;i<3;i++){
		  //将数据放入总线
		 if (ucDataCmd&0x80){
			 HT1621_DAT_HIGH();
		 }else{
			 HT1621_DAT_LOW();
		 }
		  //发送写脉冲
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
 
 //完成写一个字节数据
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

	  //延时使LCD工作电压稳定
	 delay_ms(200); 
	 Ht1621WrCmd(BIAS);
	 
	 //使用内部振荡器
	 Ht1621WrCmd(RC256); 
	 Ht1621WrCmd(SYSDIS);
	 Ht1621WrCmd(WDTDIS);
	 Ht1621WrCmd(SYSEN);
	 
	 for (i=0;i<10;i++){
		 HT1621_CS_LOW();
         //往RAM的地址4写数据  这里的参数范围为:0-31，用到的就0-9,0-3为设置温度区域，4-7为实际温度，8-9为图形控制区域
		 Ht1621WrDataCmd(i);  
		 Ht1621WrByte(0x0);
		 HT1621_CS_HIGH();
		 delay_us(HT1621_Delay); 
	 } 
	 
	 Ht1621WrCmd(LCDON);
 }
 

