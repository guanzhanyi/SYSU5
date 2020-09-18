//多线程web服务器

#include <stdio.h>  
#include <string.h>  
#include <arpa/inet.h>  
#include <pthread.h>  
#include <ctype.h>  
#include <unistd.h>  
#include <fcntl.h>  
  
#include "wrap.h"  
  
#define MAXLINE 8192  
#define SERV_PORT 8000  
  
struct s_info {                     //定义一个结构体, 将地址结构跟cfd捆绑  
    struct sockaddr_in cliaddr;  
    int connfd;  
};  

void *do_work(void *arg){
    int n=0,i=0;
    struct s_info *ts=(struct s_info *)arg;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];

    while(1){
        n=Read(ts->connfd,,buf,MAXLINE);
        if(n==0){
            printf ("the client %d closed...\n",ts->connfd);
            break;
        }
        /*新型网路地址转化函数inet_pton和inet_ntop
        这两个函数是随IPv6出现的函数，对于IPv4地址和IPv6地址都适用，函数中p和n分别代表表达（presentation)和数值（numeric)。地址的表达格式通常是ASCII字符串，数值格式则是存放到套接字地址结构的二进制值
        */

        printf ("received from %s at PORT %d\n",inet_ntop(AF_INET,&(*ts).cliaddr,sin_addr,str,sizeof(str)),ntohs((*ts).cliaddr.sin_port));

        for(i=0;i<n;i++)
            buf[i]=toupper(buf[i]);

        Write(STDOUT_FILENO,buf,n);
        Write(ts->connfd,buf,n);
    }
    Close(te->connfd);

    return (void*)0;
}

int main(){
    struct sockaddr_in serv_addr,cliaddr;
    socklen_t cliaddr_len;  
    int listenfd, connfd;  
    pthread_t tid;  
  
    struct s_info ts[256];      //创建结构体数组.  
    int i = 0;  
  
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);                     //创建一个socket, 得到lfd  
  
    bzero(&servaddr, sizeof(servaddr));                             //地址结构清零  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);                               //指定本地任意IP  
    servaddr.sin_port = htons(SERV_PORT);                                       //指定端口号   
  
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));             //绑定  
  
    Listen(listenfd, 128);                                                      //设置同一时刻链接服务器上限数  
    printf("Accepting client connect ...\n");  

    while(1){
        client_len=sizeof(cliaddr);
        connfd=Accept(listenfd,(struct sockaddr *)&cliaddr, &cliaddr_len);

        ts[i].cliaddr=cliaddr;
        ts[i].connfd=connfd;

        pthread_create(&tid,NULL,do_work,(void*)&ts[i]);
        pthread_detach(tid);

        i++;
    }
    return 0;
}





