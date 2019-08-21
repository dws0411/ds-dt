#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <errno.h>


#define PORT 6666
#define ADDR "172.28.164.151" 
#define BACK_LOG 5

SQLHENV henv;
SQLHDBC hdbc;
SQLHSTMT hstmt;
RETCODE retcode;

void insert_into_db(char* sql){
    char sourceName[128] = "odbc_demodb";  //服务器名称
    char userName[128] = "informix";
    char password[128] = "informix";

    retcode = SQLAllocHandle(SQL_HANDLE_ENV,NULL,&henv);
    if(retcode != SQL_SUCCESS){
        printf("分配环境句柄错误");
        return;
    }
    //设置环境句柄ODBC版本
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3,SQL_IS_INTEGER);
    if(retcode != SQL_SUCCESS){
        printf("设置环境句柄错误\n");
        return;
    }

    retcode = SQLAllocHandle(SQL_HANDLE_DBC,henv,&hdbc);
    if (retcode != SQL_SUCCESS)
    {
        printf("分配连接句柄错误\n");
    }
    retcode = SQLConnect(hdbc,
                        (SQLCHAR *)sourceName,
                        SQL_NTS,
                        (SQLCHAR *)userName,
                        SQL_NTS,
                        (SQLCHAR *)password,
                        SQL_NTS
    );
    if (retcode != SQL_SUCCESS)
    {
        
        printf("链接数据源错误\n");
        printf("ret = %d\n",retcode);
        /*
            此处调用SQLGetDiagRec函数查链接错误，并打印出来，可参考
            https://blog.csdn.net/kingmax26/article/details/6012790
        */
        char V_OD_stat[10]; //status SQL
        SQLINTEGER V_OD_err;
        char V_OD_msg[200];
        SQLSMALLINT V_OD_mlen;
        SQLGetDiagRec(SQL_HANDLE_DBC,hdbc,1,V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
        printf("%s(%d)",V_OD_msg,V_OD_err);

        return;
    }

    SQLAllocHandle(SQL_HANDLE_STMT,hdbc,&hstmt);

    retcode = SQLExecDirect(hstmt,
                            (SQLCHAR *)sql,
                            SQL_NTS
    );
    if(retcode != SQL_SUCCESS){
        printf("执行SQL错误!\n");
        return;
    }
    SQLFreeHandle(SQL_HANDLE_STMT,hstmt);       //释放语句句柄
    SQLDisconnect(hdbc);    
    SQLFreeHandle(SQL_HANDLE_DBC,hdbc);         //释放连接句柄
    SQLFreeHandle(SQL_HANDLE_ENV,henv);         //释放环境句柄
    return;

}

int main(){
    int iSocket;
    iSocket = socket(AF_INET,SOCK_STREAM,0);
    if (iSocket < 0)
    {
        printf("创建socket失败！ \n");
    }

    struct sockaddr_in stLocalAddr;     //建立本地socket结构体
    stLocalAddr.sin_family = AF_INET;
    stLocalAddr.sin_port = htons(PORT);
    stLocalAddr.sin_addr.s_addr = inet_addr("192.168.245.129");         //此处不能为本地ip"127.0.0.1",可参考https://blog.csdn.net/test1280/article/details/80642847

    int ret = bind(iSocket,(void *)&stLocalAddr,sizeof(stLocalAddr));   //绑定
    if (ret < 0)
    {
        printf("绑定本地ip和端口失败！\n");
        return 0;
    }else
    {
        printf("绑定成功!\n");
    }

    if (listen(iSocket,BACK_LOG) < 0)        //开启监听
    {
        printf("监听失败！\n");
        return 0;
    }

    struct sockaddr_in stRemoteAddr = {0};      //保存clinet端socket结构体
    socklen_t socklen = 0;
    int accept_socket = accept(iSocket,(void *)&stRemoteAddr, &socklen); //接受函数
    
    if (accept_socket < 0)
    {
        printf("接受失败！ \n");
        fprintf(stderr,"accept fail: error =%d(%s)!!!\n",errno,strerror(errno));
        return 0;
    }else
    {
        printf("接收成功！ \n");
    }

    char sql[2048];
    while (1)
    {
        ret = recv(accept_socket,sql,sizeof(sql),0);    //循环接收
    
        if (ret <= 0)
        {
            printf("接受失败或对端关闭连接！\n");
            break;
        }else
        {
            printf("成功接受到sql语句!\n");
        }
        ret = insert_into_db(sql);
        if(ret == 1){
            send(accept_socket,"success in odbc",sizeof("success in odbc"),0);
        }else{
            send(accept_socket,"error in odbc!",sizeof("error in odbc!"),0);
            break;
        }
    }
    close(accept_socket);
    close(iSocket);
    return 0;
    
    

}
