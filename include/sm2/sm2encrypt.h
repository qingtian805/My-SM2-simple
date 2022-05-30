#ifndef __SM2_ENC_H__
#define __SM2_ENC_H__

#define __SM2_DEBUG_ENC__

#include "sm2config.h"

//SM2椭圆曲线算法信息加密函数
//输入：message 被加密的信息
//  messagelen 加密信息长度
//  xBn,yBn 接收者公钥
//输出：emessage 加密后信息，请准备比原信息长97长度的字节串。
//返回：int 0 加密成功 -1 出现错误
int encryptMessage(char* messsage, int messagelen, char *xBn, char *yBn, char *emessage);

//SM2椭圆曲线算法信息解密函数
//输入：emessage 要解密的信息
//  emessagelen 加密信息长度
//  dBn 接收者的私钥
//输出：message 解密后的信息，长度比加密信息短97字节
//返回：int 0 解密成功 -1 出现错误
int decryptMessage(char *emessage, int emessagelen, char *dBn, char *message);

#endif