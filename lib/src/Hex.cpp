#include "HEX.h"
#include <string>
using namespace std;

//--------------------------HexXor----------------------------
/*HexXor: Xor two hex string;return the result;
*/
string HexXor(string s1,string s2)
{
	int len1,len2,n,i=0;
	char a,b,c;

	len1=s1.size();
	len2=s2.size();

	if (len1>len2)
		n=len2;
	else
		n=len1;
	string result(n,'1');

	for (i=0;i<n;i++)
	{
		//��S1[i]��ʮ�����Ƶ��ַ�ֵת��Ϊ����ֵ��
		if (s1[i]>='A' && s1[i]<='F')
			a=s1[i]-'A'+0XA;
		else if (s1[i]>='a' && s1[i]<='f')
			a=s1[i]-'a'+0XA;
		else
			a=s1[i]-'0';

		//��S2[i]��ʮ�����Ƶ��ַ�ֵת��Ϊ����ֵ��
		if (s2[i]>='A' && s2[i]<='F')
			b=s2[i]-'A'+0XA;
		else if (s2[i]>='a' && s2[i]<='f')
			b=s2[i]-'a'+0XA;
		else
			b=s2[i]-'0';

		//�����������ֵת��Ϊʮ�����Ƶ��ַ�ֵ��
		c=a^b;
		if (c>=0XA)
			result[i]='A'+c-0XA;
		else
			result[i]='0'+c;
	}

	return result;
}
