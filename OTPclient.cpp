#include <iostream>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "TOTP.h"

using namespace std;
using namespace TOTP;

typedef void (*sighandler_t)(int);

void signalHandler(int signum)
{
    cout << endl;
    exit(0);
}

int main(void)
{
    char key[] = "SVUKRBXFLEOFZGQM";//演示用Key，可以更改从文件获取
    char cotp[7];//TOTP计算结果
    time_t t;//当前时间
    int tremain;//剩余时间

    signal(SIGINT,signalHandler);//注册信号处理函数

    TOTP::TOTP(key,cotp);//计算TOTP数值

    cout << "----------TOTP验证码计算程序----------" << endl;
    cout << "**请使用ctrl + c退出程序" << endl;
    cout << "以下是您的TOTP验证码，剩余时间结束前有效" << endl;
    cout << "剩余时间\tTOTP验证码" << endl;

    t = time(NULL);//获取当前时间戳
    tremain = 30 - (t % 30);//计算剩余时间

    if(tremain >= 10)
    {
        cout << tremain << "\t\t" << cotp << flush;
    }
    else
    {
        cout << "0" << tremain << "\t\t" << cotp << flush;
    }

    while (true)
    {
        tremain--;
        sleep(1);
        if (tremain == 0)//如果剩余时间结束，再次计算TOTP
        {
            TOTP::TOTP(key,cotp);
            tremain = 30;
        }

        if(tremain >= 10)
        {
            cout << "\r" << tremain << "\t\t" << cotp << flush;
        }
        else
        {
            cout << "\r0" << tremain << "\t\t" << cotp << flush;
        }
    }
}