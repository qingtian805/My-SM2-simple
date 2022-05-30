#ifndef __SM2_SS_H__   
#define __SM2_SS_H__

#include "sm2type.h"

namespace SM2
{
    //sm3补足函数，将比特流转换为字符串
    //输入：stream 比特流
    //  streamlen 比特流长度(字节)
    //输出：string 可打印字符串
    //请确保字符串存储区长度为原长度的两倍+1
    //例：长度为32字节的流需要65字节长的字符串
    void streamToString(byte *stream, int streamlen, char *string);

    //输入：string 字符串
    //  stringlen 字符串去\0长度(字节)
    //输出：stream 比特流
    //请确保比特流区域大于stringlen/2的最小整数
    void stringToStream(char *string, int stringlen, byte *stream);

    /*
    //输入：string 表示16进制的字符串
    //     stringlen 字符串去\0长度
    //输出：ostring -1后的字符串
    void stringsub1(char* string,int stringlen,char* ostring);
    */
}

#endif