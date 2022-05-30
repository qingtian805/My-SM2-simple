#include <math.h>
#include <memory.h>
#include "sm3.h"
#include "sm2KDF.h"
#include "sm2type.h"

using namespace SM2;

//顶函数使用math.h double cile()
//底函数使用math.h double floor()

void SM2::KDF(char *Z, int Zlen, int klen, char *key)
{
    unsigned int ct = 1;
    int cel = (int)ceil((double)klen / 256.0);      //顶函数 klen/v
    int flr = (int)floor((double)klen / 256.0);     //底函数 klen/v
    int _Halen;
    byte Ha[cel][32]; //Hai
    byte Htmp[Zlen + sizeof(int)];
    byte _Ha[32]; //Ha!



    for (int i = 0; i < cel; i++){
        memcpy(Htmp, Z, Zlen);                  //Htmp = Z
        memcpy(Htmp + Zlen, &ct, sizeof(int));  //Htmp = Z||ct
        sm3(Htmp, Zlen + sizeof(int), Ha[i]);   //H(Z||ct)

        ct = ct + 1;
    }

    if (cel == flr){                         //顶底相等，则为整数
        _Halen = 32;                         //_Ha长度为32字节
        memcpy(_Ha, Ha[cel - 1], 32);        //Ha!=Ha[klen/v]top
    }
    else {
        int bl = (klen % 256) / 8;             //超出多少字节
        int l = klen % 8;                      //超出长度(比特数)
        if (l == 0){                           //若整除，则复制bl字节防止溢出
            _Halen = bl;                      //_Ha长度为bl
            memcpy(_Ha, Ha[cel - 1], bl);     //若整除，则复制bl字节防止溢出
        }
        else {                                  //不整除，多复制一个字节并处理得到多余位
            _Halen = bl + 1;                    //_Ha长度为bl+1
            memcpy(_Ha, Ha[cel - 1], bl + 1);   //复制bl+1字节到Ha!
            _Ha[bl] = _Ha[bl] >> 8 - l;         //右移位丢掉低位
            _Ha[bl] = _Ha[bl] << 8 - l;         //左移位恢复
        }
    }

    for(int i = 0; i < cel - 1; i++)               //Ha1-Ha[klen/v]-1
        memcpy(key + (i * 32), Ha[i], 32);

    memcpy(key + ((cel - 1) * 32), _Ha, _Halen);    //Ha![klen/v]
}

#if DEBUG
using namespace std;
int main(void)
{
    char key[34]="";
    char Z[4] = "abc";
    KDF(Z,3,272,key);
    return 0;
}
#endif