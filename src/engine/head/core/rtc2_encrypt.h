#pragma once

#include <string>
//#include "md5.h"
using namespace std; 

namespace rt2_core {

enum    {ENCRYPT,DECRYPT};
//enum    bool{false,true}; // if bool is not supported,use this or just replace with char
                            // and use 1 for true,0 for false;
//////////////////////////////////////////////////////////////////////////

// Type��ENCRYPT:����,DECRYPT:����
// ���������(Out)�ĳ��� >= ((datalen+7)/8)*8,����datalen�������8�ı�������С������
// In ����= Out,��ʱ��/���ܺ󽫸������뻺����(In)������
// ��keylen>8ʱϵͳ�Զ�ʹ��3��DES��/����,����ʹ�ñ�׼DES��/����.����16�ֽں�ֻȡǰ16�ֽ�

bool RtDes(char *Out,char *In,long datalen,const char *Key,int keylen,bool Type = ENCRYPT);

//////////////////////////////////////////////////////////////////////////


// ------------------------------------------------------DES---------------------------------------------------

}