#include <iostream>
#include <time.h>
#include <memory.h>
#include <string.h>
#include "sm3.h"
#include "TOTP.h"

using namespace std;

int TOTP::__pow(void)
{
    int res = 10;
    for(int i = 1; i < TOTP_DIGEST; i++)
        res = res * 10;
    
    return res;
}

void TOTP::__TC(char *ct)
{
    time_t t;
    char *tp;
    t = time(NULL);//获得当前时间戳
    t = (t-TOTP_T0)/TOTP_T1;
    tp = (char*)&t;
    for(int i=0; i<8;i++)
        ct[i] = *(tp+i);
}

void TOTP::__HMAC(char *_K,int Klen,char *M,int Mlen,char *res)
{
    unsigned char ipad = 0x5C;//内部填充
    unsigned char opad = 0x36;//外部填充
    unsigned char Hk[TOTP_HLEN];//摘要存储区

    int KLEN;//信息增加长度
    if (Mlen > TOTP_HLEN) 
        KLEN = Mlen;
    else
        KLEN = TOTP_HLEN;

    unsigned char K[TOTP_MAX_KLEN+KLEN];//信息

    //初始化K，补长0
    memset(K,0,TOTP_MAX_KLEN+Mlen);//第一次计算使用前TOTP_MAX_KLEN+Mlen字节
    memcpy(K,_K,Klen);

    for(int i = 0; i < TOTP_MAX_KLEN; i++)//K ^ ipad
        K[i] = K[i] ^ ipad;

    memcpy(K + TOTP_MAX_KLEN, M, Mlen);

    sm3(K, TOTP_MAX_KLEN + Mlen, Hk);//H(K^ipad)= hash sum 1

    //第二次初始化K，补长0
    memset(K, 0, TOTP_MAX_KLEN + TOTP_HLEN);//第二次使用全部TOTP_MAX_KLEN+摘要长度 字节
    memcpy(K, _K, Klen);

    for(int i = 0;i < TOTP_MAX_KLEN; i++)//K ^ opad
        K[i] = K[i] ^ opad;

    memcpy(K + TOTP_MAX_KLEN, Hk, TOTP_HLEN);//o key pad || hash sum 1

    sm3(K, TOTP_MAX_KLEN + TOTP_HLEN, (unsigned char*)res);
}

void TOTP::__Truncate(char *HMAC,char *cotp)
{
    short offset;
    unsigned int otp;
    unsigned int binary = 0;
    unsigned int otpp;

    offset = *(HMAC + TOTP_HLEN - 1) & 0xf;//offset 取最后一个字节的低4位
    binary = (HMAC[offset] & 0x7f) << 24;
    binary = binary | ((HMAC[offset + 1] & 0xFF)  << 16);
    binary = binary | ((HMAC[offset + 2] & 0xFF)  << 8);
    binary = binary | ((HMAC[offset + 3] & 0xFF) );
    otp = binary % (TOTP::__pow());
    otpp = otp;

    for(int i = 1; i <= TOTP_DIGEST; i++){
        cotp[TOTP_DIGEST - i] = otpp % 10 + 48;
        otpp = otpp / 10;
    }
    cotp[TOTP_DIGEST]='\0';
}

bool TOTP::TOTP(char *K,char *cotp)//K -> cotp
{
    /*
    TOTP总调用函数，如果需要修改返回位数，请修改源码最上方设置。另外，请注意字符串结尾\0问题
    */
    int klen = strlen(K);
    if (klen > TOTP_MAX_KLEN){
        cout << "KEY TOO LONG!" << endl;
        return false;
    }
    char ct[8];
    char res[TOTP_HLEN];
    TOTP::__TC(ct);
    TOTP::__HMAC(K, klen, ct, 8, res);
    TOTP::__Truncate(res, cotp);
    return true;
}

#ifdef DEBUG
int main(void){
    char key[] = "SVUKRBXFLEOFZGQMC";
    char cotp[7];
    TOTP(key,cotp);
    cout << cotp << endl;
    return 0;
}
#endif