#ifndef __SM2_KDF_H__
#define __SM2_KDF_H__

namespace SM2
{
    //密钥派生函数
    //输入：Z 共享秘密
    //  Zlen 共享秘密长度
    //  klen 要求密钥长度(比特)
    //输出：key 求出的密钥
    void KDF(char *Z, int Zlen, int klen, char *key);
}

#endif