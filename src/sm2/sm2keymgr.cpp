extern "C"
{
#include "miracl.h"
}
#include "sm2keymgr.h"
#include "sm2calculators.h"
#include "sm2init.h"
#include <iostream>
#include "sm2StreamString.h"
#include "sm2type.h"

using std::cout;
using std::endl;
using namespace SM2;

void SM2::gen_keypair(big d, big Px, big Py)
{
    genRandom2(d);//生成处于[1,n-2]的随机数

    calP1(d, Px, Py);
}

bool SM2::verify_pubkey(big Px, big Py)
{
    bool ret = true;
    epoint *P;
    big tmp1;
    big tmp2;
    tmp1 = mirvar(0);
    tmp2 = mirvar(0);

    P = epoint_init();
    
    if (! epoint_set(Px,Py,0,P)){
        ret = -1;
        goto EXIT_VP;
    }

    //step 1 p infinity
    if (point_at_infinity(P)){
        ret = false;
        goto EXIT_VP;
    }
    
    //step 2 in [0,p-1]
    if (! in_0p1(Px) || !in_0p1(Py)){
        ret = false;
        goto EXIT_VP;
    }
    
    //step 3 yp^2=xp^3+axp+b
    pow(Py, 2, tmp1);//计算左值
    calfumula1(Px, tmp2);//计算右值

    if (! is_equal(tmp1, tmp2)){
        ret = false;
        goto EXIT_VP;
    }

    //step 4 [n]P infinity
    calnP(Px, Py, tmp1, tmp2);
    epoint_set(tmp1, tmp2, 0, P);

    if (point_at_infinity(P))
        ret = false;

EXIT_VP:
    mirkill(tmp1);
    mirkill(tmp2);
    return ret;
}

int gen_keypair(char *dn, char *Pxn, char *Pyn)
{
    miracl *mip;
    if (! init_miracl(mip)){
        cout << "MIRACL INIT FALTAL" << endl;
        return -1;
    }
    init_base();

    big d;
    big Px;
    big Py;
    d = mirvar(0);
    Px = mirvar(0);
    Py = mirvar(0);

    SM2::gen_keypair(d, Px, Py);

    big_to_bytes(0, d, dn, FALSE);
    big_to_bytes(0, Px, Pxn, FALSE);
    big_to_bytes(0, Py, Pyn, FALSE);
    streamToString((byte*)dn, 32, dn);
    streamToString((byte*)Pxn, 32, Pxn);
    streamToString((byte*)Pyn, 32, Pyn);

EXIT_FG:
    mirkill(d);
    mirkill(Px);
    mirkill(Py);
    mirexit();
    return 0;
}

int verify_pubkey(char *Pxn, char *Pyn)
{
    miracl *mip;
    if (! init_miracl(mip)){
        cout << "MIRACL INIT FALTAL" << endl;
        return -1;
    }
    init_base();

    int ret;
    big Px;
    big Py;
    Px = mirvar(0);
    Py = mirvar(0);

    cinstr(Px, Pxn);
    cinstr(Py, Pyn);

    ret = SM2::verify_pubkey(Px, Py);

    mirkill(Px);
    mirkill(Py);
    return ret;
}