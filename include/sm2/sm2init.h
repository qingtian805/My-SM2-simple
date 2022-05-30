#ifndef __SM2_INIT_H__
#define __SM2_INIT_H__

extern "C"
{
#include "miracl.h"
}

namespace SM2
{
    //初始化miracl系统
    //输入/输出：mip 指向miracl系统的指针
    //    返回： 初始化结果，成功为true，失败为false
    bool init_miracl(miracl *mip);
    
    //初始化SM2系统基础数据结构，包括椭圆曲线和参数
    //参数从设置中调用，无需输入输出返回
    void init_base(void);

    //清理SM2系统基础结构，功能为清理参数和结构本身
    void exit_base(void);
}

#endif