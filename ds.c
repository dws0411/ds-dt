#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>

#define PORT 6666
#define ADDR "192.168.245.129"

int main(){
    int iSocket;    //句柄
    iSocket = socket(AF_INET,SOCK_STREAM,0);
    if (iSocket < 0)
    {
        printf("创建socket失败！\n");
    }

    struct sockaddr_in remoteAddr = {0};      //connect函数所用结构体赋值
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(PORT);
    unsigned int iremodeAddr = 0;
    inet_pton(AF_INET,ADDR,&iremodeAddr);
    remoteAddr.sin_addr.s_addr = iremodeAddr;

    if(connect(iSocket,(void *)&remoteAddr,sizeof(remoteAddr))){
        printf("连接失败！ \n");
        fprintf(stderr,"socket connect error=%d(%s)!!!\n",errno,strerror(errno));
    }else
    {
        printf("连接成功！\n");
    }
    
    char sql[2048];         //要发送的sql
    scanf("%[^\n]",sql);   
    
    send(iSocket,sql,strlen(sql),0);    //发送sql

    char recvMessage[255];         //接受server端回复的消息
    int ret = recv(iSocket,recvMessage,255,0);
    if (ret > 0)
    {
        printf("接受到来自server端的回复：%s \n",recv);
    }
    close(iSocket);
    return 0;
    

}
