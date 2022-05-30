#ifndef __SM2_CALC_H__
#define __SM2_CALC_H__

extern "C"{
#include "miracl.h"
}
#include "sm2type.h"

namespace SM2
{
    struct sm2parameter{
        big A;
        big B;
        big P;
        big N;
        big Gx;
        big Gy;
    };

    extern struct sm2parameter *__SM2_GLOBAL_PARAMETER__;
    
    //计算_M = ZA||M
    //输入：ZA 用户其他信息，sm3哈希值，32长度字节串
    //  message 签名信息，字节串
    //  mlen 签名信息长度(字节)
    //输出：_message  上划线M的对应值
    void gen_Message(char *ZA, byte *message, int mlen, byte *_message);

    //计算e = H256(_M)，并转化整数
    //输入：_message 上划线M的对应值
    //  _mlen 上划线M的长度(字节)
    //输出：e 转换为整数后的e
    void calE(byte *_message, int _mlen, big e);

    //生成随机数k in [1,n-1]
    //编译器定义：SEED 随机数生成种子
    //从系统获取：time 时间，当作种子
    //输出：k 大小位于[1,n-1]的整数随机数
    void genRandom(big k);

    //生成随机数k in [1,n-2]
    //编译器定义：SEED 随机数生成种子
    //从系统获取：time 时间，作为种子
    //输出：k 大小位于[1,n-2]的整数随机数
    void genRandom2(big k);

    //计算r = (e + x1)mod n
    //计算R = (e`+ x1`)mod n
    //计算t = (r`+ s`)mod n
    void calrt(big er, big x1s, big rt);

    //计算s==((1 + dA)^ -1·(k - r·dA))mod n
    void cals(big k, big dA, big r, big s);

    //计算椭圆曲线点P2(x2,y2) = [k]P1(x1,y1)
    //计算椭圆曲线点P2(x2,y2) = [k]PB(xB,yB)
    //如果是压缩点，则将x同时作为y传入
    void calP(big k, big x1, big y1, big x2, big y2);
    
    //计算签名公钥 P1(x1,y1) = [k]G
    //是calP的封装
    //输入：k
    //输出：x1 取出的坐标x值
    //     y1 取出的坐标y值
    //如果是压缩点，则将x同时作为y传入
    inline void calP1(big k, big x1, big y1){
        calP(k, __SM2_GLOBAL_PARAMETER__->Gx, __SM2_GLOBAL_PARAMETER__->Gy, x1, y1);
    }

    //计算椭圆曲线点 P2(x2,y2)=[n]P1(x1,y1)
    //是calP的封装
    //输入：x1 y1
    //输出：x2 y2
    inline void calnP(big x1, big y1, big x2, big y2){
        calP(__SM2_GLOBAL_PARAMETER__->N, x1, y1, x2, y2);
    }

    //计算椭圆曲线点P1`(x1`,y1`)=[s`]G+[t]PA
    //输入：s t xA yA
    //输出：x1 y1
    //如果是压缩点，则将x同时作为y传入
    void cal_P1(big s, big t, big xA, big yA, big x1, big y1);

    //计算y = x^3 + ax + b(mod p)
    //输入：x a b
    //输出：y
    //从系统参数中提取：p
    void calfumula_1(big x, big a, big b, big y);

    //计算y = x^3 + ax + b(mod p)
    //是calfumula_1的进一步封装
    //输入：x
    //输出：y
    //从系统参数中提取:a b p
    inline void calfumula1(big x, big y){
        calfumula_1(x, __SM2_GLOBAL_PARAMETER__->A, __SM2_GLOBAL_PARAMETER__->B, y);
    }

    //计算y = x^n mod p
    //是对power函数的封装
    //输入：x n
    //输出：y
    //从系统中获取 p
    inline void pow(big x, long n, big y){
        power(x, n, __SM2_GLOBAL_PARAMETER__->P, y);
    }
    
    //输入：x 要判断的数值
    //     n 区间小
    //     m 区间大
    //输出：x 是否处于区间 [n,m] 内，是则返回真
    bool in_nm(big __x, big __n, big __m);

    //返回：判断输入数值是否处于[1,n-1]，在则返回真
    bool in_1n1(big __x);

    //返回：判断输入数值是否处于[1,p-1]，在则返回真
    bool in_0p1(big __x);

    //返回：判断输入是否等于n，相等则返回真
    inline bool is_n(big __x){
        return mr_compare(__x, __SM2_GLOBAL_PARAMETER__->N) == 0;// == n
    }

    //返回：判断给出数值是否为0，为0返回真
    bool is_zero(big __x);

    //返回：判断输入是否相等，相等返回真
    inline bool is_equal(big __x,big __y){
        return (mr_compare(__x, __y) == 0);
    }

    //返回：判断字节串是否为全0,是则返回真
    bool is_allzero(byte *Bs,int lenB);
}

#endif