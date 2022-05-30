extern "C"
{
#include "miracl.h"
}
#include "sm2calculators.h"
#include "sm2config.h"
#include "sm2StreamString.h"
#include <time.h>
#include <memory.h>
#include "sm3.h"

using namespace SM2;

struct sm2parameter *SM2::__SM2_GLOBAL_PARAMETER__;

#define PARA SM2::__SM2_GLOBAL_PARAMETER__

void SM2::gen_Message(char *ZAn, byte *message, int mlen, byte *_message)
{
    byte ZA[32];
    stringToStream(ZAn, 64, ZA);
    memcpy(_message, ZAn, 32);//偏移0-31复制为ZA
    memcpy(_message+32, message, mlen);//偏移32-++复制为message
}

void SM2::calE(byte *_message, int _mlen, big e)
{
    char en[32];
    sm3(_message, _mlen, (byte*)en);
    bytes_to_big(32, en, e);
}

void SM2::genRandom(big k)
{
    do {
        irand(time(NULL) + __SM2_SEED__);
        bigrand(PARA->N, k);// 0 <= k < n
    } while (is_zero(k));//是0则重新生成
}

void SM2::genRandom2(big k)
{
    big n;
    n = mirvar(0);

    copy(PARA->N, n);
    decr(n, 1, n); //n = n - 1
    
    do {
        irand(time(NULL) + __SM2_SEED__);
        bigrand(n, k);// 0 <= k < n, n=n-1
    } while (is_zero(k));//是0则重新生成

    mirkill(n);
}

void SM2::calrt(big er, big x1s, big rt)
{
    big tmp;

    tmp = mirvar(0);

    add(x1s, er, tmp);//tmp = x1 + e
    divide(tmp, PARA->N, PARA->N);//tmp = tmp mod n
    copy(tmp, rt);//r = tmp

    mirkill(tmp);
}

void SM2::cals(big k, big dA, big r, big s)
{
    big tmp;
    big und;

    tmp = mirvar(0);
    und = mirvar(0);
    
    incr(dA, 1, und);//und = 1+dA
    xgcd(und, PARA->N, und, und, und);//und = und^-1 mod n
    mad(r, dA, dA, PARA->N, PARA->N, tmp);//tmp = (r * dA) mod n
    if (mr_compare(k, tmp) >= 0){ //tmp = k - tmp
        subtract(k, tmp, tmp);
    }
    else{
        subtract(PARA->N, tmp, tmp);
        add(k, tmp, tmp);
    }
    mad(tmp, und, und, PARA->N, PARA->N, tmp); //tmp = (tmp * und) mod n
    copy(tmp, s);
    mirkill(tmp);
    mirkill(und);
}

void SM2::calP(big k, big x1, big y1, big x2, big y2)
{
    epoint *p1;
    epoint *p2;

    p1 = epoint_init();
    p2 = epoint_init();

    epoint_set(x1, y1, 0, p1);//设置点
    
    ecurve_mult(k, p1, p2);//计算点乘p2=[k]p1

    epoint_get(p2, x2, y2);//取出坐标

    epoint_free(p1);
    epoint_free(p2);
}

//inline void SM2::calP1(big k,big x1,big y1)

//inline void SM2::calnP(big x1, big y1, big x2, big y2)

void SM2::cal_P1(big s, big t, big xA, big yA, big x1, big y1)
{
    epoint *g;
    epoint *pA;
    epoint *p1;

    g = epoint_init();
    pA = epoint_init();
    p1 = epoint_init();
    epoint_set(PARA->Gx, PARA->Gy, 0, g);//g = (xG,yG)
    epoint_set(xA, yA, 0, pA);//pA = (xA,yA)
    
    ecurve_mult2(s, g, t, pA, p1);//p1 = [s]g + [t]pA
    epoint_get(p1, x1, y1);

    epoint_free(g);
    epoint_free(pA);
    epoint_free(p1);
}

void SM2::calfumula_1(big x, big a, big b, big y)
{
    power(x, 3, PARA->P, y);//y=x^3 mod p

    add(y, b, y);//y=y+b
    divide(y, PARA->P, PARA->P);//y=y mod p

    mad(x, a, y, PARA->P, PARA->P, y);//y = ax+y mod p
}

//inline void SM2::calfumula1(big x, big y)

//inline void SM2::pow(big x, long n, big y)

bool SM2::in_nm(big __x, big __n, big __m)
{
    bool res1,res2;
    res1 = (mr_compare(__x, __n) >= 0);//x >= 0
    res2 = (mr_compare(__x, __m) <= 0);//x <= n
    return res1 && res2; // >=n && <=m
}

bool SM2::in_1n1(big __x)
{
    big cmp1;//1
    big cmpn;//n
    cmp1 = mirvar(1);
    cmpn = mirvar(0);

    copy(PARA->N, cmpn);
    decr(cmpn,1,cmpn);

    bool res = SM2::in_nm(__x,cmp1,cmpn);

    mirkill(cmp1);
    mirkill(cmpn);
    return res;
}

bool SM2::in_0p1(big __x)
{
    bool res;
    big cmp0;
    big cmpp;
    cmp0 = mirvar(0);
    cmpp = mirvar(0);
    
    copy(PARA->P, cmpp);
    decr(cmpp, 1, cmpp);

    res = SM2::in_nm(__x, cmp0, cmpp);

    mirkill(cmp0);
    mirkill(cmpp);
    return res;
}

//inline bool SM2::is_n(big __x)

bool SM2::is_zero(big __x)
{
    big cmp0;
    bool res;

    cmp0 = mirvar(0);

    res = (mr_compare(__x, cmp0) == 0);// == 0

    mirkill(cmp0);
    return res;
}

//inline bool SM2::is_equal(big __x, big __y)

bool SM2::is_allzero(byte* Bs, int lenB)
{
    for(int i = 0;i < lenB;i++){
        if(*(Bs + i) == 0)//如果当前为0,则检查下一位
            continue;
        else//如果当前位不是0,则返回false并退出函数
            return false;
    }
    return true;//如果检查结束，则全部为0
}