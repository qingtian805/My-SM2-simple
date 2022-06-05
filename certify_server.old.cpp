#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <unistd.h>
#include "sm2encrypt.h"
#include "sm4.h"
#include "TOTP.h"
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

    char user[20];
    char key[] = "SVUKRBXFLEOFZGQM"; //演示用key，可以从文件中读取
    //服务器密钥
    char privateK[] = "3945208F7B2144B13F36E38AC6D39F95889393692860B51A42FB81EF4DF7C5B8";
    char comkey[] = "COMKEYDEMOKEVIN0";//会话密钥

    int randnum;//随机数
    char totpn[7];//TOTP随机数
    int tsize;//传输大小，用于解决SM4分组16字节问题。
    std::string sm4code;

    //step 0 init
    //初始化sm2 sm4系统和socket套接字
    sm4 s;
    s.setType(sm4::ECB);
    s.setIv("1234567890123456");//无用设置，用来跳过检查

    sm2cfg *sm2p;
    sm2p = sm2init();
    if (sm2p == NULL){
        std::cout << "System init failed!" << std::endl;
        return 1;
    }

    int serv_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);
    bind(serv_sock,(sockaddr*)&serv_addr,sizeof(serv_addr));

    listen(serv_sock,5);

    sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);

    int clnt_sock = accept(serv_sock,(sockaddr*)&clnt_addr,&clnt_addr_len);

    //step 0.5 recv 1 + sm2enc(rand | username)
    read(clnt_sock,buffer,sizeof(DataGram));

    memcpy(&data.pNumber,buffer,sizeof(short));
    memcpy(&data.datalen,buffer+sizeof(int),sizeof(int));
    memcpy(data.data,buffer+sizeof(int)+sizeof(int),data.datalen +1);

    decryptMessage(data.data,data.datalen,privateK,buffer);
    buffer[data.datalen - 97] = '\0';

    memcpy(&randnum,buffer,sizeof(int));
    memcpy(user,buffer+sizeof(int),data.datalen-sizeof(int));

    if(data.pNumber != HELLO)
    {
        std::cout << "Client Protocal Error" << std::endl;
        goto EXIT;
    }
    if(strcmp(user,"kevin226") != 0){
        std::cout << "log: Invalid user:" << user << "try to login" << std::endl;
        goto EXIT;
    }
    s.setKey(key);//将密钥设置为用户密钥

    //step 1 1+sm4enc(rand | Request TOTP)
    data.pNumber = HELLO;
    data.datalen = sizeof(int) + 12;

    tsize = ceil(data.datalen/16.0) * 16;

    memcpy(buffer, &randnum, sizeof(int));
    memcpy(buffer + sizeof(int), "Request TOTP", 13);

    //sm4code = buffer;
    //sm4code = s.encrypt(sm4code);
    //memset(data.data, 0, 1024);
    memcpy(data.data, s.encrypt(buffer).c_str(), tsize);
    data.data[tsize] = '\0';
    
    write(clnt_sock, &data, sizeof(data));

    //step 1.5 recv 2 + sm2enc(rand | TOTP)
    read(clnt_sock, buffer, sizeof(DataGram));

    memcpy(&data.pNumber, buffer, sizeof(short));
    memcpy(&data.datalen, buffer+sizeof(int), sizeof(int));
    memcpy(data.data, buffer+sizeof(int)+sizeof(int), data.datalen +1);

    strcpy(privateK,"3945208F7B2144B13F36E38AC6D39F95889393692860B51A42FB81EF4DF7C5B8");
    decryptMessage(data.data, data.datalen, privateK, buffer);
    buffer[data.datalen - 97] = '\0';
    memcpy(&randnum, buffer, sizeof(int));

    TOTP::TOTP(key, totpn);
    if(strcmp(buffer+sizeof(int), totpn) != 0)
    {

        std::cout << "Wrong TOTP!" << std::endl;
        goto EXIT;
    }

    //step 2 2 + sm4enc(rand | comkey)
    data.pNumber = TOTP_CERTIFY;
    data.datalen = sizeof(int) + 16;

    tsize = (int)(ceil(data.datalen/16.0) * 16);

    memcpy(buffer, &randnum, sizeof(int));
    memcpy(buffer + sizeof(int), comkey, sizeof(comkey));

    //sm4code = buffer;
    memcpy(data.data, s.encrypt(buffer).c_str(), tsize);
    data.data[tsize] = '\0';

    write(clnt_sock, &data, sizeof(data));
    s.setKey(comkey);

    //step 2.5 recv 4 + sm4enc(rand3 | OK)
    read(clnt_sock, buffer, sizeof(DataGram));

    memcpy(&data.pNumber, buffer, sizeof(short));
    memcpy(&data.datalen, buffer + sizeof(int), sizeof(int));
    tsize = (int)(ceil(data.datalen/16.0) * 16);
    memcpy(data.data, buffer + sizeof(int) + sizeof(int), tsize + 1);

    s.setKey(comkey);
    memcpy(buffer, s.decrypt(data.data).c_str(), data.datalen);
    buffer[data.datalen] = '\0';

    memcpy(&randnum, buffer, sizeof(int));
    if(strcmp(buffer+sizeof(int), "OK") != 0)
    {
        std::cout << "Key exchange failure!" << std::endl;
        goto EXIT;
    }

    //step 3
    data.pNumber = KEY_EXCHANGE;
    data.datalen = sizeof(int) + 2;

    tsize = (int)(ceil(data.datalen/16.0) * 16);

    memcpy(buffer, &randnum, sizeof(int));
    memcpy(buffer+sizeof(int), "OK", 3);

    memcpy(data.data, s.encrypt(buffer).c_str(), tsize);
    data.data[tsize] = '\0';

    write(clnt_sock, &data, sizeof(data));

EXIT:
    sm2exit(sm2p);
    close(serv_sock);
    close(clnt_sock);
    return 0;
}