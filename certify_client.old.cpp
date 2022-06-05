#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "sm2encrypt.h"
#include "sm4.h"
#include <math.h>

#define HELLO 1
#define TOTP_CERTIFY 2
#define KEY_EXCHANGE 4
//#define CONTINUE 8

struct DataGram
{
    short pNumber;
    int datalen;
    char data[568];
};

int main(void)
{
    DataGram data;
    char buffer[sizeof(DataGram)];

    char user[] = "kevin226";
    char key[] = "SVUKRBXFLEOFZGQM"; //演示用key，可以从文件中读取
    //服务器公钥，存储在所有用户的程序中
    char publicX[] = "09F9DF311E5421A150DD7D161E4BC5C672179FAD1833FC076BB08FF356F35020";
    char publicY[] = "CCEA490CE26775A52DC6EA718CC1AA600AED05FBF35E084A6632F6072DA9AD13";

    int randnum;//随机数
    int Randnum;//验证用随机数
    char totpn[6];//TOTP随机数
    char comkey[17];//会话密钥
    std::string sm4code;//sm4返回的string
    int tsize;

    memset(comkey,0,17);

    //step0
    //初始化sm2 sm4系统
    sm4 s;
    s.setType(sm4::ECB);
    s.setKey(key);
    s.setIv("1234567890123456");//无用设置，用来跳过检查
    
    sm2cfg *sm2p;
    sm2p = sm2init();
    if (sm2p == NULL){
        std::cout << "System init failed!" << std::endl;
        return 1;
    }

    //创建socket
    int sock = socket(AF_INET, SOCK_STREAM,0);

    //设置服务器地址
    sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);
    connect(sock,(sockaddr*)&serv_addr,sizeof(serv_addr));
    
    //step 1 1+sm2enc
    data.pNumber = HELLO;//设置协议内容
    data.datalen = sizeof(user) + sizeof(int) + 97;

    srand(time(NULL)); //设置随机数种子
    randnum = rand();//生成随机数

    //构建加密信息
    memcpy(buffer,&randnum,sizeof(int));
    memcpy(buffer+sizeof(int),user,sizeof(user));

    encryptMessage(buffer,data.datalen-97,publicX,publicY,data.data);//加密信息并输入data字段
    data.data[data.datalen] = '\0';

    write(sock, &data, sizeof(data));

    //step 1.5
    read(sock, &buffer, sizeof(DataGram));

    //还原数据
    memcpy(&data.pNumber,buffer,sizeof(short));
    memcpy(&data.datalen,buffer+sizeof(int),sizeof(int));
    tsize = (int)ceil(data.datalen/16.0)*16;
    memcpy(data.data, buffer+sizeof(int)+sizeof(int), tsize + 1);
    
    memcpy(buffer,s.decrypt(data.data).c_str(),data.datalen);//解密数据，存入buffer
    buffer[data.datalen] = '\0';
    //验证数据
    if(data.pNumber != HELLO)
    {
        std::cout << "Received a wrong portal" << std::endl;
        goto EXIT;
    }
    if(*(int*)buffer != randnum)//如果两次随机数不同，则退出
    {
        std::cout << "certify failed! Server sent an wrong message" << std::endl;
        goto EXIT;
    }

    //step 2
    std::cout << "Server connected!" << std::endl;
    std::cout << "Now please input your TOTP number" << std::endl;
    
    for(int i = 0;i<6;i++)
    {
        totpn[i] = getchar();
    }

    data.pNumber = TOTP_CERTIFY;
    data.datalen = sizeof(int)+6+97;//sm2加密后长度增加97字节

    srand(time(NULL));
    //randnum = rand();
    randnum = 0x11111111;

    memcpy(buffer,&randnum,sizeof(int));
    memcpy(buffer+sizeof(int),totpn,6);

    encryptMessage(buffer,sizeof(int)+6,publicX,publicY,data.data);
    data.data[data.datalen] = '\0';

    write(sock, &data, sizeof(data));

    //step 2.5
    read(sock, buffer, sizeof(DataGram));

    //还原数据
    memcpy(&data.pNumber,buffer,sizeof(short));
    memcpy(&data.datalen,buffer+sizeof(int),sizeof(int));
    tsize = (int)ceil(data.datalen/16.0)*16;
    memcpy(data.data,buffer+sizeof(int)+sizeof(int),tsize + 1);

    memcpy(buffer,s.decrypt(data.data).c_str(),data.datalen);
    buffer[data.datalen] = '\0';
    if(data.pNumber != TOTP_CERTIFY)
    {
        std::cout << "Received a wrong portal" << std::endl;
        goto EXIT;
    }
    if(*(int*)buffer != randnum)//如果两次随机数不同，则退出
    {
        std::cout << "certify failed! Server sent an wrong message" << std::endl;
        goto EXIT;
    }

    memcpy(comkey,buffer+sizeof(int),17);

    //step 3
    std::cout << "TOTP certify successeed" << std::endl;
    std::cout << "Received Key:" << comkey << std::endl;

    data.pNumber = KEY_EXCHANGE;
    data.datalen = sizeof(int) + 2;

    tsize = (int)ceil(data.datalen/16.0)*16;

    sleep(1);//睡眠保证时间不同
    srand(time(NULL));
    randnum = rand();

    std::cout << randnum << std::endl;

    memcpy(buffer,&randnum,sizeof(int));
    memcpy(buffer+sizeof(int),"OK",3);

    s.setKey(comkey);
    //sm4code = s.encrypt(buffer);
    memcpy(data.data,s.encrypt(buffer).c_str(),tsize+1);
    data.data[tsize] = '\0';

    write(sock, &data,sizeof(data));

    //step 3.5
    read(sock,buffer,sizeof(DataGram));

    memcpy(&data.pNumber,buffer,sizeof(short));
    memcpy(&data.datalen,buffer+sizeof(int),sizeof(int));
    tsize = (int)ceil(data.datalen/16.0)*16;
    memcpy(data.data,buffer+sizeof(int)+sizeof(int),tsize +1);

    memcpy(buffer,s.decrypt(data.data).c_str(),data.datalen);
    buffer[data.datalen] = '\0';
    if(data.pNumber != KEY_EXCHANGE)
    {
        std::cout << "Received a wrong portal" << std::endl;
        goto EXIT;
    }
    if(*(int*)buffer != randnum)//如果两次随机数不同，则退出
    {
        std::cout << "certify failed! Server sent an wrong message" << std::endl;
        goto EXIT;
    }

    std::cout << "Key Exchanged. Establishing communication" << std::endl;

EXIT:
    sm2exit(sm2p);
    close(sock);
    return 0;
}