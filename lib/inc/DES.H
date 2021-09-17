

#ifndef _DES_H
#define _DES_H

#include <string>
using  std :: string;

string DesRun(string PlainString,string keyString,bool model);
string TDesRun(string PlainString,string keyString,bool model);
string MAC(string plaintext,string keyString,string InitString);
void ASCHex2Bool_1(string str1,int *bstr);

#endif 



