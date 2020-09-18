//select()实现多路转接服务器
#include<sys/select.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdio.h>

#define MAXLINE 80
#define SERV_PORT 9876

int main(){
    int i,j,n,maxi;
    int maxfd,listenfd,connfd,sockfd;
    int nready,client[FD_SETSIZE];//自定义数组client,防止遍历1024哥文件描述符
    char buf[BUFSIZ],str[INET_ADDRSTRLEN];

    struct sockaddr_in cli_addr,serv_addr;
    socklen_t cli_addr_len;
    fd_set after_set,before_set;

    listenfd =Socket(AF_INET,SOCK_STREAM,0);
    int opt=1;
    // SO_REUSEADDR 允许重用本地IP和端口
    //SOL_SOCKET 套接字级别
    //获取或者设置与某个套接字关联的选项。
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);                               //
    serv_addr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    Listen(listenfd, 128);                                                      // 设置

    maxfd=listenfd;

    maxi=-1;
    for(i=0;i<FD_SETSIZE;i++){          //＃define __FD_SETSIZE 1024
        client[i]=-1;
    }

    FD_ZERO(&before_set);
    FD_SET(listenfd, &before_set);

    while(1){
        after_set=before_set;

        nready=select(maxfd+1,&after_set,NULL,NULL,NULL);
        if(nready<0)
            perr_exit("select error");
        
        if(FD_ISSET(listenfd, &after_set)){
            cli_addr_len=sizeof(cli_addr);
            connfd=Accept(listenfd,(struct sockaddr *)&cli_addr,&cli_addr_len);
            printf ("reveived from %s at PORT %d\n",inet_ntop(AF_INET,&cli_addr.sin_addr,str,sizeof(str)),ntohs(cli_addr.sin_port));
        }
        //
        for(i=0;i<FD_SETSIZE;i++)
            if(client[i]<0){
                client[i]=connfd;//找client[i]中没有使用的位置
                break;          //保存accept返回的文件描述到client[]里
            }
        //达到select能监控的文件个数上限        
        if(i==FD_SETSIZE){
            fputs("too many clietns\n",stderr);
            exit(1);
        }

        FD_SET(connfd,&before_set);//向监控文件描述符集合allset添加新的文件描述符connfd
        if(connfd>maxfd)
            maxfd=connfd;   // select第一个参数需要
        if(i>maxi)
            maxi=i;//保证maxi存的总是client[]的最后一个元素的下标
        
        if(--nready==0)
            continue;
        
        for(i=0;i<=maxi;i++){
            if((sockfd=client[i])<0)
                continue;
            if((FD_ISSET(sockfd,&after_set))){
                if((n=Read(sockfd,buf,sizeof(buf)))==0){//当client关闭连接时,服务器也相应的关闭连接
                    Close(sockfd);
                    FD_CLR(sockfd,&before_set);//删除select对此文件描述符的监控
                }
                else if(n>0){
                    for(j=0;j<n;j++){
                        buf[j]=toupper(buf[j]);
                    }
                    Write(sockfd,buf,n);
                    Write(STDOUT_FILENO,buf,n);
                }
                if(--nready==0)
                    break;//跳出for中,但是还在while中
            }
        }
    }
    Close(listenfd);
    return 0;

}