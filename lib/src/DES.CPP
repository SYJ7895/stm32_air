#include "DES.h"
#include "HEX.h"
#include <string>
#include <iostream>

using namespace std;

//IP[64]:Initial Permutation table
const unsigned char IP[64]=
{
58, 50, 42, 34, 26, 18, 10, 2,
60, 52, 44, 36, 28, 20, 12, 4,
62, 54, 46, 38, 30, 22, 14, 6,
64, 56, 48, 40, 32, 24, 16, 8,
57, 49, 41, 33, 25, 17,   9, 1,
59, 51, 43, 35, 27, 19, 11, 3,
61, 53, 45, 37, 29, 21, 13, 5,
63, 55, 47, 39, 31, 23, 15, 7
};

//Anti_IP[64]:anti initial permutation table
const unsigned char Anti_IP[64]=
{
40, 8, 48, 16, 56, 24, 64, 32,
39, 7, 47, 15, 55, 23, 63, 31,
38, 6, 46, 14, 54, 22, 62, 30,
37, 5, 45, 13, 53, 21, 61, 29,
36, 4, 44, 12, 52, 20, 60, 28,
35, 3, 43, 11, 51, 19, 59, 27,
34, 2, 42, 10, 50, 18, 58, 26,
33, 1, 41,  9, 49, 17, 57, 25
};

//Extending and permutating table
const unsigned char Extend[48]=
{
32,  1,  2,  3,  4,  5, 
4,   5,  6,  7,  8,  9,
8,   9, 10, 11, 12, 13, 
12, 13, 14, 15, 16, 17,
16, 17, 18, 19, 20, 21, 
20, 21, 22, 23, 24, 25,
24, 25, 26, 27, 28, 29, 
28, 29, 30, 31, 32,  1
};

//Permutation table
const unsigned char Permutation[32]=
{
16,  7, 20, 21, 29, 12, 28, 17,
 1, 15, 23, 26,  5, 18, 31, 10,
 2,  8, 24, 14, 32, 27,  3,  9,
19, 13, 30,  6, 22, 11,  4, 25
};

//PC-1
const unsigned char PC_1[56]=
{
57, 49, 41, 33, 25, 17,  9,
 1, 58, 50, 42, 34, 26, 18,
10,   2, 59, 51, 43, 35, 27,
19, 11,   3, 60, 52, 44, 36,
63, 55, 47, 39, 31, 23, 15,
 7, 62, 54, 46, 38, 30, 22,
14,   6, 61, 53, 45, 37, 29, 
21, 13,   5, 28, 20, 12,  4
};

//PC-2
const unsigned char PC_2[48]=
{
14, 17, 11, 24,  1,  5,  3, 28, 
15,  6, 21, 10, 23, 19, 12,  4, 
26,  8, 16,  7, 27, 20, 13,  2,
41, 52, 31, 37, 47, 55, 30, 40, 
51, 45, 33, 48, 44, 49, 39, 56, 
34, 53, 46, 42, 50, 36, 29, 32
};
//LeftMove
const unsigned char LeftMove[16]={
1, 1, 2, 2, 2, 2, 2, 2,
1, 2, 2, 2, 2, 2, 2, 1
};

const static char S_Box[8][4][16] =
{
{
   // S1 
   {14,   4, 13,   1,   2, 15, 11,   8,   3, 10,   6, 12,   5,   9,   0,   7},
   { 0, 15,   7,   4, 14,   2, 13,   1, 10,   6, 12, 11,   9,   5,   3,   8},
   { 4,   1, 14,   8, 13,   6,   2, 11, 15, 12,   9,   7,   3, 10,   5,   0},
      {15, 12,   8,   2,   4,   9,   1,   7,   5, 11,   3, 14, 10,   0,   6, 13}
},
{
   // S2 
      {15,   1,   8, 14,   6, 11,   3,   4,   9,   7,   2, 13, 12,   0,   5, 10},
   { 3, 13,   4,   7, 15,   2,   8, 14, 12,   0,   1, 10,   6,   9, 11,   5},
   { 0, 14,   7, 11, 10,   4, 13,   1,   5,   8, 12,   6,   9,   3,   2, 15},
      {13,   8, 10,   1,   3, 15,   4,   2, 11,   6,   7, 12,   0,   5, 14,   9}
},
{
   // S3 
      {10,   0,   9, 14,   6,   3, 15,   5,   1, 13, 12,   7, 11,   4,   2,   8},
   {13,   7,   0,   9,   3,   4,   6, 10,   2,   8,   5, 14, 12, 11, 15,   1},
   {13,   6,   4,   9,   8, 15,   3,   0, 11,   1,   2, 12,   5, 10, 14,   7},
      { 1, 10, 13,   0,   6,   9,   8,   7,   4, 15, 14,   3, 11,   5,   2, 12}
},
{
   // S4 
      { 7, 13, 14,   3,   0,   6,   9, 10,   1,   2,   8,   5, 11, 12,   4, 15},
   {13,   8, 11,   5,   6, 15,   0,   3,   4,   7,   2, 12,   1, 10, 14,   9},
   {10,   6,   9,   0, 12, 11,   7, 13, 15,   1,   3, 14,   5,   2,   8,   4},
      { 3, 15,   0,   6, 10,   1, 13,   8,   9,   4,   5, 11, 12,   7,   2, 14}
      },
      {
   // S5 
      { 2, 12,   4,   1,   7, 10, 11,   6,   8,   5,   3, 15, 13,   0, 14,   9},
   {14, 11,   2, 12,   4,   7, 13,   1,   5,   0, 15, 10,   3,   9,   8,   6},
   { 4,   2,   1, 11, 10, 13,   7,   8, 15,   9, 12,   5,   6,   3,   0, 14},
      {11,   8, 12,   7,   1, 14,   2, 13,   6, 15,   0,   9, 10,   4,   5,   3}
      },
      {
   // S6 
      {12,   1, 10, 15,   9,   2,   6,   8,   0, 13,   3,   4, 14,   7,   5, 11},
   {10, 15,   4,   2,   7, 12,   9,   5,   6,   1, 13, 14,   0, 11,   3,   8},
   { 9, 14, 15,   5,   2,   8, 12,   3,   7,   0,   4, 10,   1, 13, 11,   6},
      { 4,   3,   2, 12,   9,   5, 15, 10, 11, 14,   1,   7,   6,   0,   8, 13}
      },
      {
   // S7 
      { 4, 11,   2, 14, 15,   0,   8, 13,   3, 12,   9,   7,   5, 10,   6,   1},
   {13,   0, 11,   7,   4,   9,   1, 10, 14,   3,   5, 12,   2, 15,   8,   6},
   { 1,   4, 11, 13, 12,   3,   7, 14, 10, 15,   6,   8,   0,   5,   9,   2},
      { 6, 11, 13,   8,   1,   4, 10,   7,   9,   5,   0, 15, 14,   2,   3, 12}
      },
      {
   // S8 
      {13,   2,   8,   4,   6, 15, 11,   1, 10,   9,   3, 14,   5,   0, 12,   7},
   { 1, 15, 13,   8, 10,   3,   7,   4, 12,   5,   6, 11,   0, 14,   9,   2},
   { 7, 11,   4,   1,   9, 12, 14,   2,   0,   6, 10, 13, 15,   3,   5,   8},
      { 2,   1, 14,   7,   4, 10,   8, 13, 15, 12,   9,   0,   3,   5,   6, 11}
      }
};
bool SubKey[16][48]; //存贮每轮生产的48位子密钥.
int OperateKey(bool *ResultKey);
int IP_Function(bool *plaintext);
int AntiIP_Function(bool *plaintext);
int F_Function(bool *plaintext,int n);
int S_Function(bool *RightExtend,bool *RightTemp);
void ASCHex2Bool(string str1,bool *bstr);
void ASCHex2Bool_1(string str1,int *bstr);
string Bool2ASCHex(bool *ciphertext);
int DesEncrypt(bool *plaintext);
int DesDecrypt(bool *ciphertext);


//----------------------------MAC-----------------------------
/*
   MAC: get mac.plaintext input data;keyString key;
   InitString Initial data;
   return 4 byte MAC.
*/
string MAC(string plaintext,string keyString,string InitString)
{
	string temp;
	int i,length,keyLen,numBlock;

	length=plaintext.size();
	keyLen=keyString.size();

	//最后一块补齐。
	if (length%16==0)
		plaintext+="8000000000000000";
	else
	{
		i=0;
		while(i<(16-length%16))
		{
			if (i==0)
				{
					plaintext+="80";
					i+=2;
				}
				else
				{
					plaintext+="0";
					i++;
				}
		}
	}

	length=plaintext.size();
	numBlock=length/16;

	for (i=0;i<numBlock;i++)
	{
		temp=plaintext.substr(i*16,16);
		InitString=HexXor(InitString,temp);
		if (keyLen==16)
			InitString=DesRun(InitString,keyString,0);
		else
			InitString=DesRun(InitString,keyString.substr(0,16),0);
	}

	if (keyLen==32)
	{
		InitString=DesRun(InitString,keyString.substr(16,16),1);
		InitString=DesRun(InitString,keyString.substr(0,16),0);
	}
    
	
	return InitString.substr(0,8);
}

//----------------------------TDesRun-----------------------------
/* TDesRun 3Des Encrypt or 3Des Decrypt.
   if model=0 3Des Encrypt,plainString is plaintext and return ciphertext;
   if model=1 3Des Decrypt,plainString is ciphertext and return plaintext;
   KeyString is aways key;
  if program running is wrong return a string that is empty;
*/
string TDesRun(string PlainString,string keyString,bool model)
{
	string tempString,KeyA,KeyB;
	int length;

	length=keyString.size();
	if (length!=32)
	{
		cout<<"密钥长度错\n";
		return NULL;
	}
	KeyA=keyString.substr(0,16);
	KeyB=keyString.substr(16,16);

	if (model==false)   //3DES加密
	{
		tempString=DesRun(PlainString,KeyA,false);
		tempString=DesRun(tempString,KeyB,true);
		tempString=DesRun(tempString,KeyA,false);
	}
	else             //3DES解密
	{
		tempString=DesRun(PlainString,KeyA,true);
		tempString=DesRun(tempString,KeyB,false);
		tempString=DesRun(tempString,KeyA,true);
	}
	return tempString;
}

//----------------------------DesRun-----------------------------
/* DesRun Des Encrypt or Des Decrypt.
   if model=0 Des Encrypt,plainString is plaintext and return ciphertext;
   if model=1 Des Decrypt,plainString is ciphertext and return plaintext;
   KeyString is aways key;
  if program running is wrong return a string that is empty;
*/
string DesRun(string PlainString,string keyString,bool model)
{
	string CipherString,tempString,plainTemp;
	bool plaintext[64],key[64],ciphertext[64];
	int length,i,n,j;


	//处理密钥，得到子密钥。
	ASCHex2Bool(keyString,key);
	OperateKey(key);
	
	//加密运算
	if (model==false)
	{
		length=PlainString.size();
	    for (i=0;i<length;i+=16)
		{
		    n=length-i;
		    if (n<16)   //最后不足16个十六进制字母即8字节，填充。
			{
			    tempString=PlainString.substr(i,n);
			    j=0;
			    while (n+j<=16)
				{
				    if (j==0)
					{
					    tempString+="80";
					    j+=2;
					}
				    else
					{
					    tempString+="0";
					    j=j+1;
					}
				} // end of while
			}// end of if(n<16) 
		    else
			    tempString=PlainString.substr(i,16);

		    ASCHex2Bool(tempString,plaintext);
		    DesEncrypt(plaintext);
		    CipherString=CipherString+Bool2ASCHex(plaintext);
		}// end of for
		
		return CipherString;
	}// end of if(model==0)
	else	//解密。
	{
		CipherString=PlainString;
		length=CipherString.size();
	    for (i=0;i<length;i+=16)
		{
		    tempString=CipherString.substr(i,16);
		    ASCHex2Bool(tempString,ciphertext);
		    DesDecrypt(ciphertext);
	        plainTemp=plainTemp+Bool2ASCHex(ciphertext);
		}
		return plainTemp;
	}
}

//---------------------------------------------------------
int DesDecrypt(bool *ciphertext)
{
	int i,j,state;
	bool tempPlain[64],tempR[32];
	
	state=IP_Function(ciphertext);
	if (state!=0)
		return 1;
	for (i=0;i<16;i++)
	{
		memset(tempR,0,32);
		memset(tempPlain,0,64);
		memcpy(tempR,ciphertext+32,32);
		state=F_Function(tempR,15-i);
		if (state!=0)
			return 1;	
		//Left and Right changing
		if (i!=15)
		{
			memcpy(tempPlain,ciphertext+32,32); //copy R(i-1) to L(i);
		    for (j=0;j<32;j++)    //L(i-1) XOR the result of F.
			    tempPlain[j+32]=ciphertext[j] ^ tempR[j];
		    memcpy(ciphertext,tempPlain,64);
		}
		else              //i=15,Left and Right do not change
		{
			for (j=0;j<32;j++)    
			    tempPlain[j]=ciphertext[j] ^ tempR[j];
			memcpy(ciphertext,tempPlain,32);
		}
	}
	state=AntiIP_Function(ciphertext);
	if (state!=0)
		return 1;
	return 0;
}
//----------------------------DesEncrypt--------------------
//Des encrypt
/* parameters:  bool *plaintext: The data is encrypted and its length is 64 bit.
                                 At last if encrpting is ok,plaintext is ciphertext.
                bool *key  the key is used in DEs encrypting and its length also is 64 bit.
   return values:  0:DES encrypting is ok 1:DES encrypting is wrong!
*/
int DesEncrypt(bool *plaintext)
{
	int i,j,state;
	bool tempPlain[64],tempR[32];
	
	state=IP_Function(plaintext);
	if (state!=0)
		return 1;
	for (i=0;i<16;i++)
	{
		memset(tempR,0,32);
		memset(tempPlain,0,64);
		memcpy(tempR,plaintext+32,32);
		state=F_Function(tempR,i);
		if (state!=0)
			return 1;	
		//Left and Right changing
		if (i!=15)
		{
			memcpy(tempPlain,plaintext+32,32); //copy R(i-1) to L(i);
		    for (j=0;j<32;j++)    //L(i-1) XOR the result of F.
			    tempPlain[j+32]=plaintext[j] ^ tempR[j];
		    memcpy(plaintext,tempPlain,64);
		}
		else              //i=15,Left and Right do not change
		{
			for (j=0;j<32;j++)    
			    tempPlain[j]=plaintext[j] ^ tempR[j];
			memcpy(plaintext,tempPlain,32);
		}
	}
	state=AntiIP_Function(plaintext);
	if (state!=0)
		return 1;
	return 0;
}

//----------------------------OperateKey--------------------
/* main idea: DES encrypting Operating key;
   parameters:  Key:input key.Left moving and produce subkey.
*/
int  OperateKey(bool *Key)
{
	bool ResultKey[56],temp[28];
	int i,n,j;

	for (i=0;i<56;i++)             //key permutationg changing one;
		ResultKey[i]=Key[PC_1[i]-1];
	
	for (i=0;i<16;i++)
	{
		n=LeftMove[i];

		if (n==1)
		{
			memset(temp,0,28);
		    memcpy(temp,ResultKey,28);  //C(i-1) left moving
		    memcpy(ResultKey,temp+1,27);
	        ResultKey[27]=temp[0];

			memset(temp,0,28);
		    memcpy(temp,ResultKey+28,28);  //D(i-1) left moving
		    memcpy(ResultKey+28,temp+1,27);
		    ResultKey[55]=temp[0];
		}
	    else if (n==2)
		{
			memset(temp,0,28);
		    memcpy(temp,ResultKey,28);   //C(i)-1 left moving
		    memcpy(ResultKey,temp+2,26);
		    ResultKey[26]=temp[0];
		    ResultKey[27]=temp[1];

			memset(temp,0,28);
		    memcpy(temp,ResultKey+28,28);   //D(i)-1 left moving
		    memcpy(ResultKey+28,temp+2,26);
		    ResultKey[54]=temp[0];
		    ResultKey[55]=temp[1];
		}
		else
			return 1;
		
	    for (j=0;j<48;j++)    //PC-2
			SubKey[i][j]=ResultKey[PC_2[j]-1];
	}
	return 0;
}
//----------------------------IP_Function--------------------
//Initial permutation at the beginning of DES encrypting.
//parameter: bool* plaintext 

int IP_Function(bool *plaintext)
{
	bool temp[64];
	int i;

	for (i=0;i<64;i++) //permute by variable temp
		temp[i]=plaintext[IP[i]-1];
	memcpy(plaintext,temp,64);    //copy temp to plaintext	
	return 0;
}

//-----------------------------AntiIP_Function--------------------
//permutation at the end of DES Encrypting with the table of Anti_IP
//Parameter: bool *plaintxt, The data is permuted with the table of Anti_IP

int AntiIP_Function(bool *plaintext)
{
	bool temp[64];
	int i;

	for (i=0;i<64;i++) //permute by variable temp	
		temp[i]=plaintext[Anti_IP[i]-1];
	memcpy(plaintext,temp,64);   //copy temp to plaintext	
	return 0;
}

//-----------------------------F_Function--------------------
//DES encrypt function of F
int F_Function(bool *plaintext,int n)
{
	bool Right_Extend[48],RightTemp[32];
	int i,state;

	for (i=0;i<48;i++)           //Extend Right half. 
	{
		Right_Extend[i]=*(plaintext+Extend[i]-1);
	}

	for (i=0;i<48;i++) //Extend XOR key
		Right_Extend[i] ^=SubKey[n][i];

    state=S_Function(Right_Extend,RightTemp);  //F function;
	if (state!=0)
		return 1;
	for (i=0;i<32;i++)
		plaintext[i]=RightTemp[Permutation[i]-1];
	return 0;
}

//
int S_Function(bool *RightExtend,bool *RightTemp)
{
	int i,j,line, column,value;
	bool temp[6];

	for (i=0;i<8;i++)
	{
		for (j=0;j<6;j++)
		{
			temp[j]=*(RightExtend+j+i*6);
		}
		line=temp[0]*2+temp[5];
		column=temp[1]*8+temp[2]*4+temp[3]*2+temp[4]*1;
		value=S_Box[i][line][column];

		if (value>=8)     //Transate the value of S_Box to four binary value;
		{
			*(RightTemp+i*4)=true;
			value=value%8;
		}
		else
			*(RightTemp+i*4)=false;

		if (value>=4)
		{
			*(RightTemp+i*4+1)=true;
			value=value%4;
		}
		else
			*(RightTemp+i*4+1)=false;

		if (value>=2)
		{
			*(RightTemp+i*4+2)=true;
			value=value%2;
		}
		else
			*(RightTemp+i*4+2)=false;

		if (value>=1)
		{
			*(RightTemp+i*4+3)=true;
			value=value%1;
		}
		else
			*(RightTemp+i*4+3)=false;

	}
	return 0;
}

//将输入的'0'-'F'转化为4位二进制。
void ASCHex2Bool(string str1,bool *bstr)
{
	int i,n=0;

	for (i=0;i<16;i++)
	{
		//获取ASC对应的16进制值。
		if (str1[i]>='A' && str1[i]<='F')
		{
			n=0;
			n=0xA+str1[i]-'A';
		}
		if (str1[i]>='a' && str1[i]<='f')
		{
			n=0;
			n=0xA+str1[i]-'a';
		}
		if (str1[i]>='0' && str1[i]<='9')
		{
			n=0;
			n=str1[i]-'0';
		}

		//将对应值转化为4位二进制。
		if (n>=8)
		{
			bstr[i*4]=true;
			n=n%8;
		}
		else
			bstr[i*4]=false;
		if (n>=4)
		{
			bstr[i*4+1]=true;
			n=n%4;
		}
		else
			bstr[i*4+1]=false;
		if (n>=2)
		{
			bstr[i*4+2]=true;
			n=n%2;
		}
		else
			bstr[i*4+2]=false;
		if (n>=1)
			bstr[i*4+3]=true;
		else
			bstr[i*4+3]=false;
	}
}

//将4位二进制转化为'0'-'F'
string Bool2ASCHex(bool *ciphertext)
{
	int i,n;
	string cipherString(16,'1');
	
	for (i=0;i<64;i+=4)
	{
		n=0;
		n=ciphertext[i]*8+ciphertext[i+1]*4+ciphertext[i+2]*2+ciphertext[i+3]*1;
		if (n>=10)
			cipherString[i/4]='A'+n-10;
		else
			cipherString[i/4]='0'+n;
	}
	return cipherString;
}
//将输入的'0'-'F'转化为4位二进制。
void ASCHex2Bool_1(string str1,int *bstr)
{
	int i,n=0;

	for (i=0;i<16;i++)
	{
		//获取ASC对应的16进制值。
		if (str1[i]>='A' && str1[i]<='F')
		{
			n=0;
			n=0xA+str1[i]-'A';
		}
		if (str1[i]>='a' && str1[i]<='f')
		{
			n=0;
			n=0xA+str1[i]-'a';
		}
		if (str1[i]>='0' && str1[i]<='9')
		{
			n=0;
			n=str1[i]-'0';
		}

		//将对应值转化为4位二进制。
		if (n>=8)
		{
			bstr[i*4]=1;
			n=n%8;
		}
		else
			bstr[i*4]=0;
		if (n>=4)
		{
			bstr[i*4+1]=1;
			n=n%4;
		}
		else
			bstr[i*4+1]=0;
		if (n>=2)
		{
			bstr[i*4+2]=1;
			n=n%2;
		}
		else
			bstr[i*4+2]=0;
		if (n>=1)
			bstr[i*4+3]=1;
		else
			bstr[i*4+3]=0;
	}

}
