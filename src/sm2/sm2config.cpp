#include <sm2config.h>
#include <memory.h>
#include <stdlib.h>
#include "sm2StreamString.h"

struct sm2cfg *__SM2_GLOBAL_CONF__;

sm2cfg *sm2init(void)
{
    sm2cfg *sm2p = (sm2cfg*)malloc(sizeof(sm2cfg));
    //内存不足，分配失败
    if (sm2p == NULL){
        return sm2p;
    }
    //将默认参数输入设置
    sm2p->POINT_TYPE = 4;
    memcpy(sm2p->P, __SM2_P__, 32);
    memcpy(sm2p->A, __SM2_A__, 32);
    memcpy(sm2p->B, __SM2_B__, 32);
    memcpy(sm2p->N, __SM2_N__, 32);
    memcpy(sm2p->Gx, __SM2_Gx__, 32);
    memcpy(sm2p->Gy, __SM2_Gy__, 32);

    //在系统中存储设置的指针
    __SM2_GLOBAL_CONF__ = sm2p;

    return sm2p;
}

void sm2config_b(char *sm2pp, char *p)
{
    memcpy(sm2pp, p, 32);
}

void sm2config(char *sm2pp, char *pn)
{
    char p[32];
    SM2::stringToStream(pn, 64, (unsigned char*)p);
    memcpy(sm2pp, p, 32);
}

void sm2pointcfg(int sm2PointType)
{
    __SM2_GLOBAL_CONF__->POINT_TYPE = sm2PointType;
}

void sm2exit(sm2cfg *sm2p)
{
    free(sm2p);
}