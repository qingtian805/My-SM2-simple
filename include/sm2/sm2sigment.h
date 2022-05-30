#ifndef __SM2_SIG_H__
#define __SM2_SIG_H__

#include "sm2config.h"

//计算用户其他信息函数（ZA）
//输入：ID 发送者ID
//  IDlen 发送者ID长度(字节)
//  xAn yAn 发送者公钥坐标
//输出：ZA[65] 用户其他信息
void genZA(char *ID, int IDlen, char *xAn, char *yAn, unsigned char *ZA);

//SM2椭圆曲线算法签名生成函数
//输入：ZA[65] 发送者其他信息
//  xAn[65] yAn[65] 发送者公钥坐标
//  dAn[65] 发送者私钥
//  message 签名信息
//  messagelen 信息长度(字节)，如果是字符串请考虑末尾\0的增加与否，增加则+1
//输出：rn[65] sn[65] 签名密钥对
//返回：int 0 签名生成成功 -1 出现错误
int genSignment(char *ZA, char *dAn, char *message, int messagelen,
                char *rn, char *sn);

//SM2椭圆曲线算法签名验证函数
//输入：ZA 发送者其他信息
//  xAn yAn 发送者的公钥坐标
//  message 签名信息
//  messagelen 信息长度(字节)，如果是字符串请考虑末尾\0的增加与否，增加则+1
//  rn sn 签名密钥对
//返回：int 1 签名验证成功，0 验证失败，-1 出现错误
int verifySignment(char *ZA, char *xAn, char *yAn, char *message, int messagelen,
                                char *rn, char *sn);

#endif