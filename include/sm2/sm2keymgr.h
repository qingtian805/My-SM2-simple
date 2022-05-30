#ifndef __SM2_KEYMGR_H__
#define __SM2_KEYMGR_H__

extern "C"{
#include"miracl.h"
}
#include "sm2config.h"

//生成SM2算法密钥对
//输出：dn[65] 生成的私钥
//     Pxn[65] 生成的公钥 的x值
//     Pyn[65] 生成的公钥 的y值
//返回：int 0 密钥对生成成功 -1 出现错误
int gen_keypair(char *dn, char *Pxn, char *Pyn);

//公钥验证算法
//输入：Pxn[65] 公钥的x值
//     Pyn[65] 公钥的y值
//返回：int 1 公钥通过验证 0 公钥未通过验证 -1 出现错误
int verify_pubkey(char *Pxn, char *Pyn);

namespace SM2
{
    //生成SM2算法密钥对
    //输出：R 生成的私钥 pRivate
    //     Ux 生成的公钥 pUblic 的x值
    //     Uy 生成的公钥 pUblic 的y值
    void gen_keypair(big d, big Px,big Py);

    //公钥验证算法
    //输入：Px 公钥的x值
    //     Py 公钥的y值
    //返回：公钥是否通过验证
    bool verify_pubkey(big Px, big Py);
}

#endif