extern "C"
{
#include "miracl.h"
}
#include "sm2init.h"
#include "sm2config.h"
#include "sm2calculators.h"
#include <stdlib.h>

using namespace SM2;

#define PARA __SM2_GLOBAL_PARAMETER__
#define CONF __SM2_GLOBAL_CONF__

bool SM2::init_miracl(miracl *mip)
{
    mip = mirsys(64, 16);
    if (mip == NULL)
        return false;

    mip->IOBASE = 16;
    return true;
}

void SM2::init_base(void)
{
    PARA = (sm2parameter*)malloc(sizeof(sm2parameter));
    //分配参数内存
    PARA->A = mirvar(0);
    PARA->B = mirvar(0);
    PARA->P = mirvar(0);
    PARA->N = mirvar(0);
    PARA->Gx = mirvar(0);
    PARA->Gy = mirvar(0);

    //初始化参数
    bytes_to_big(32, CONF->A, PARA->A);
    bytes_to_big(32, CONF->B, PARA->B);
    bytes_to_big(32, CONF->P, PARA->P);
    bytes_to_big(32, CONF->N, PARA->N);
    bytes_to_big(32, CONF->Gx, PARA->Gx);
    bytes_to_big(32, CONF->Gy, PARA->Gy);

    //初始化椭圆曲线
    ecurve_init(PARA->A, PARA->B, PARA->P, MR_PROJECTIVE);
}

void SM2::exit_base(void)
{
    mirkill(PARA->A);
    mirkill(PARA->B);
    mirkill(PARA->P);
    mirkill(PARA->N);
    mirkill(PARA->Gx);
    mirkill(PARA->Gy);
    free(PARA);
}