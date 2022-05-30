#ifndef __SM2_CONF_H__
#define __SM2_CONF_H__

//随机数生成器赋值
#define __SM2_SEED__ 0x1BD8C95A

//椭圆曲线参数赋值
//使用16进制字符串赋值，使用时按需转换
#define __SM2_P__  "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\
\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#define __SM2_A__  "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\
\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC"
#define __SM2_B__  "\x28\xE9\xFA\x9E\x9D\x9F\x5E\x34\x4D\x5A\x9E\x4B\xCF\x65\
\x09\xA7\xF3\x97\x89\xF5\x15\xAB\x8F\x92\xDD\xBC\xBD\x41\x4D\x94\x0E\x93"
#define __SM2_N__  "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\
\xFF\xFF\x72\x03\xDF\x6B\x21\xC6\x05\x2B\x53\xBB\xF4\x09\x39\xD5\x41\x23"
#define __SM2_Gx__ "\x32\xC4\xAE\x2C\x1F\x19\x81\x19\x5F\x99\x04\x46\x6A\x39\
\xC9\x94\x8F\xE3\x0B\xBF\xF2\x66\x0B\xE1\x71\x5A\x45\x89\x33\x4C\x74\xC7"
#define __SM2_Gy__ "\xBC\x37\x36\xA2\xF4\xF6\x77\x9C\x59\xBD\xCE\xE3\x6B\x69\
\x21\x53\xD0\xA9\x87\x7C\xC6\x2A\x47\x40\x02\xDF\x32\xE5\x21\x39\xF0\xA0"

//SM2配置结构
struct sm2cfg{
    int POINT_TYPE;
    char P[32];
    char A[32];
    char B[32];
    char N[32];
    char Gx[32];
    char Gy[32];
};

extern struct sm2cfg *__SM2_GLOBAL_CONF__;

//系统内设置指针
//struct sm2cfg *__SM2_GLOBAL_CONF__;

//用户设置接口
typedef sm2cfg sm2cfg;

/*SM2系统设置初始化函数
 *输入：sm2p 一个sm2cfg结构指针。
 *sm2cfg结构包含默认的椭圆曲线参数以及值为04点压缩方式定义
 *返回：sm2cfg* 非NULL 初始化成功 NULL 初始化失败
 */
sm2cfg *sm2init(void);

/*SM2系统参数更改函数
 *输入：char *sm2pp 从sm2cfg结构中指定的一个sm2系统参数。如sm2p->A
 *    char *p 要更改为的参数，使用十六进制字符串"\xff"表示
 */
void sm2config_b(char *sm2pp, char *p);

/*SM2系统参数更改函数
 *输入：char *sm2pp 从sm2cfg结构中指定的一个sm2系统参数。如sm2p->A
 *    char *p 要更改为的参数，使用字符串"ff"表示
 */
void sm2config(char *sm2pp, char *p);

/*SM2系统点模式设置函数（NOT WORKING）
 *输入：int sm2PointType 一个指定的SM2点模式
 *    程序会更改设置模块中的点模式
 *    请使用给定的宏进行设置，除非你知道你在做什么
 */
void sm2pointcfg(int sm2PointType);

/*SM2系统退出函数
 *输入：sm2p 一个由sm2init初始化的SM2系统设置地址
 */
void sm2exit(sm2cfg *sm2p);

#endif