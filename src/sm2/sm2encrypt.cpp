extern "C"
{
#include "miracl.h"
}
#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include "sm3.h"
#include "sm2encrypt.h"
#include "sm2type.h"
#include "sm2init.h"
#include "sm2calculators.h"
#include "sm2KDF.h"
#include "sm2StreamString.h"
#include "sm2config.h"

using std::cout;
using std::endl;
using namespace SM2;

int encryptMessage(char *message, int messagelen, char *xBn, char *yBn, char *emessage)
{
    miracl *mip;
    if (!init_miracl(mip)){
        cout << "MIRACL INIT FALTAL" << endl;
        return -1;
    }
    init_base();

    int ret = -1;
    byte x1b[32];
    byte y1b[32];//p1的比特串形态
    byte x2b[32];
    byte y2b[32];//p2的比特串形态
    byte key[messagelen];//KDF生成的密钥t，也作为加密后的C2
    byte hash[64 + messagelen];//等待hash的数据，用于计算C3和KDF
    byte C3[32];
    big k;//随机数
    big x1;
    big y1;
    big xB;
    big yB;//B的公钥
    big x2;
    big y2;
    epoint* pB;//B的公钥
    k = mirvar(0);
    x1 = mirvar(0);
    y1 = mirvar(0);
    xB = mirvar(0);
    yB = mirvar(0);
    x2 = mirvar(0);
    y2 = mirvar(0);

    pB = epoint_init();

    cinstr(xB,xBn);//初始化pB用于判断是否位于无限远点
    cinstr(yB,yBn);

    do {
        //step 1 产生随机数k
        genRandom(k);
        
        //step 2 C1 = G点乘并将结果转换为比特串
        calP1(k,x1,y1);
        big_to_bytes(0,x1,(char*)x1b,FALSE);
        big_to_bytes(0,y1,(char*)y1b,FALSE);
        
        //step 3 计算公钥是否是无限远点
        epoint_set(xB,yB,0,pB);
        if (point_at_infinity(pB)){
            cout << "ERROR! B`s public key is invalid!" << endl;
            goto EXIT_FE;
        }
        
        //step 4 计算Pb点乘，将结果转换为比特串
        calP(k, xB, yB, x2, y2);
        big_to_bytes(0, x2, (char*)x2b, FALSE);
        big_to_bytes(0, y2, (char*)y2b, FALSE);
        
        //step 5 t = KDF(x2|y2)
        memcpy(hash, x2b, 32);
        memcpy(hash + 32, y2b, 32);
        KDF((char*)hash, 64, messagelen*8, (char*)key);
    } while (is_allzero(key, messagelen));
    
    //step 6 C2 = 异或计算
    for (int i=0; i<messagelen; i++)
        key[i] = message[i] ^ key[i];
    
    //step 7 C3 = hash(x2|message|y2)
    memcpy(hash + 32, message, messagelen);
    memcpy(hash + 32 + messagelen, y2b, 32);
    sm3(hash, messagelen + 64, C3);
    
    //step 8 输出密文 C1|C3|C2
    emessage[0] = 4;//C1，按照标准，C1由两部分组成
    memcpy(emessage+1, x1b, 32);//1 点压缩格式
    memcpy(emessage+33, y1b, 32);//2 点数据

    memcpy(emessage+65, C3, 32);//C3 密文哈希值

    memcpy(emessage+97, key, messagelen);//C2 密文

    ret = 0;
EXIT_FE:
    mirkill(k);
    mirkill(x1);
    mirkill(y1);
    mirkill(xB);
    mirkill(yB);
    mirkill(x2);
    mirkill(y2);
    epoint_free(pB);
    exit_base();
    mirexit();
    return ret;
}

int decryptMessage(char *emessage, int emessagelen, char *dBn, char *message)
{
    miracl *mip;
    if (! init_miracl(mip)){
        cout << "MIRACL INIT FALTAL" << endl;
        return -1;
    }
    init_base();

    int ret = -1;
    int messagelen = emessagelen-97;//原信息长度
    char x1n[65];
    char y1n[65];
    byte x2b[32];
    byte y2b[32];
    byte hash[64+messagelen];//等待hash计算的数值
    byte key[messagelen];//KDF生成的密钥t，也作为解密后的C2
    byte u[32];
    big x1;
    big y1;
    big dB;
    big x2;
    big y2;
    epoint* p1;
    x1 = mirvar(0);
    y1 = mirvar(0);
    dB = mirvar(0);
    x2 = mirvar(0);
    y2 = mirvar(0);
    p1 = epoint_init();

    cinstr(dB,dBn);

    //step 1 功能:取出C1曲线点并验证有效性 等待被替换
    switch (emessage[0]){
    case 4:
        streamToString((byte*)emessage + 1, 32, x1n);
        streamToString((byte*)emessage + 33, 32, y1n);
        cinstr(x1, x1n);
        cinstr(y1, y1n);
        break;
    
    default:
        cout << "unsupported point format!" << endl;
        goto EXIT_FD;
    }

    if (! epoint_set(x1, y1, 0, p1)){
        cout << "ERROR! Point is not on your ecruve!" << endl;
        goto EXIT_FD;
    }

    //step 2 判断[h]p1是否为无限远点（默认h=1）
    if (point_at_infinity(p1)){
        cout << "ERROR! Using invalid public key!" << endl;
        goto EXIT_FD;
    }

    //step 3 P2=[dB]C1
    calP(dB, x1, y1, x2, y2);
    big_to_bytes(0, x2, (char*)x2b, FALSE);
    big_to_bytes(0, y2, (char*)y2b, FALSE);
    
    //step 4 t = KDF(x2|y2)
    memcpy(hash, x2b, 32);
    memcpy(hash+32, y2b, 32);
    KDF((char*)hash, 64, messagelen * 8, (char*)key);
    
    //step 5 and some of step 6 异或计算C2 t 结果存放在Step6的指定位置
    for (int i = 0;i<messagelen;i++)//hash[32~messagelen+32] = message
        hash[32 + i] = emessage[97 + i] ^ key[i];
    
    //step 6 Hash(x2|M|y2)
    memcpy(hash + messagelen + 32, y2b, 32);//hash + 32 + emessagelen - 97
    sm3(hash, messagelen + 64, u);
    if (memcmp(u, emessage + 65, 32) != 0){
        cout << "ERROR! Message may be modified!" << endl;
        goto EXIT_FD;
    }
    
    //step 7
    memcpy(message, hash + 32, messagelen);

    ret = 0;
EXIT_FD:
    mirkill(x1);
    mirkill(y1);
    mirkill(dB);
    mirkill(x2);
    mirkill(y2);
    epoint_free(p1);
    exit_base();
    mirexit();
    return ret;
}