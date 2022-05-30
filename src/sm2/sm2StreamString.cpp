#include "sm2StreamString.h"

//#define __SM2_DEBUG_SS__

#ifdef __SM2_DEBUG_SS__
#include<iostream>
#include"sm3.h"
using std::cout;
using std::endl;
#endif

using namespace SM2;

void SM2::streamToString(byte *stream,int streamlen, char *string)
{
    int pm;
    int pr;
    string[streamlen * 2] = '\0';
    pr = streamlen * 2 - 1;
    
    for (pm = streamlen - 1; pm >= 0; pm--){
        string[pr] = stream[pm] & 0b00001111;//低4位
        if(string[pr] > 9)
            string[pr] = string[pr] + 55;//55='A'-10
        else
            string[pr] = string[pr] + '0';

        pr--;
        string[pr] = stream[pm] >> 4;//高4位

        if (string[pr] > 9)
            string[pr] = string[pr] + 55;//55='A'-10
        else
            string[pr] = string[pr] + '0';

        pr--;
    }
}

void SM2::stringToStream(char *string,int stringlen,unsigned char *stream)
{
    int pm;
    int pr;
    pr = stringlen - 1;
    if (string == (char*)stream){//如果在相同地址
        pm = stringlen - 1;
        if((stringlen % 2) == 0)
            goto LOW;
    }
    else {//不在相同地址
        if ((stringlen % 2) == 0){
            pm = stringlen / 2 - 1;
            goto LOW;
        }
        else {
            pm = stringlen / 2;
        }
    }
    //处理单数情况
    if (string[pr] > '9')//高4位，低四位设为0
        stream[pm] = (string[pr] - 55) << 4;
    else
        stream[pm] = (string[pr] - '0') << 4;

    pr--;
    pm--;
    //双数情况从此开始
LOW:
    while (pr >= 0){
        if (string[pr] > '9')//低4位
            stream[pm] = string[pr] - 55;//55 = 'A'-10
        else
            stream[pm] = string[pr] - '0';

        pr--;

        if (string[pr] > '9')//高4位
            stream[pm] = stream[pm] | ((string[pr] - 55) << 4);
        else
            stream[pm] = stream[pm] | ((string[pr] - '0') << 4);
        
        pr--;
        pm--;
    }

    //相同存储区最后一步：复制内容
    if (string == (char*)stream)
    {
        pr = 0;
        pm = stringlen / 2;
        while (pm < stringlen) {
            string[pr] = string[pm];
            pr++;
            pm++;
        }
    }
}
/*
void SM2::stringsub1(char* string,int stringlen,char* ostring)
{
    int pn = stringlen - 1;

    while(true)//使用字符串处理完成n-1
    {
        ostring[pn] = string[pn] - 1;
        if(ostring[pn] < '0')//如果当前位结果<1
        {
            ostring[pn] = 'F';
            pn--;
            continue;
        }
        else if(ostring[pn] < 'A' && ostring[pn] > '9')//如果当前位结果<A
        {
            ostring[pn] = '9';
        }
        break;
    }

    if(ostring!=string)//如果是新字符串，则重新构建
    {
        ostring[stringlen] = '\0';
        for(pn--;pn>=0;pn--)
        {
            ostring[pn] = string[pn];
        }
    }
}
*/

#ifdef __SM2_DEBUG_SS__
using namespace std;
int main(){
    unsigned char b[10] = "abcdefghi";
    unsigned char res[32];//存储生成的随机字节串
    char origin[65];//存储转换后的字符串
    unsigned char test[32];//存储再次转换后的字节串
    sm3(b,9,res);

    cout << "---stream string coverting test---" << endl;
    streamToString(res,32,origin);
    cout << origin <<endl;
    stringToStream(origin,63,test);//测试不同地址情况，单数
    cout << origin << endl;
    stringToStream(origin,64,test);//测试不同地址情况，双数
    cout << origin << endl;
    stringToStream(origin,63,(byte*)origin);//相同地址情况，单数
    streamToString(res,32,origin);
    stringToStream(origin,64,(byte*)origin);//相同地址，双数

    cout << "--------string sub 1 test---------" << endl;
    char nn[65] = "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54A01";
    char nns[65];
    
    cout << nn << endl;
    stringsub1(nn,64,nn);
    cout << nn << endl;
    stringsub1(nn,64,nns);
    cout << nns << endl;
    cout << nn << endl;
    
    return 0;
}
#endif