#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>

#define PORT 6666
#define ADDR "172.28.164.151"

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
    char recvMessage[255];         //接受server端回复的消息
    while(strcmp(sql," ") != 0){        
        scanf("%[^\n]",sql);   
        if(strcmp(sql,"bye")){
            send(iSocket,sql,strlen(sql),0);    //发送sql
            getchar();                          //此处添加getchar()是为了消费语句结束后的换行符，否则循环运行到上一句无法正常输入
            int ret = recv(iSocket,recvMessage,255,0);
            if (ret > 0)
            {
                printf("接受到来自server端的回复：%s \n",recvMessage);
                memset(sql,0,sizeof(sql));          //接受到server端的回复，说明成功发送sql语句，故需将sql重置，否则无限循环会
                fflush(stdin);
            }
        }else
        {
            printf("源端接受到结束信号！\n");
            break;
        }       
    }
    close(iSocket);
    return 0;
    

}
