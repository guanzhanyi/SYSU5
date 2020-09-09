# linux网络编程基础api
## 主机字节序和网络字节序之间的转换
```
#include<netinet/in.h>
unsigned long int htonl(unsigned long int hostlong);//host to network long
unsigned short int htons(unsigned short int hostshort);//
unsigned long int ntohl(unsigned long int netlong);
unsigned short int ntohs(unsigned short int hostshort);
```

## 表示socket地址的结构体sockaddr
```
#include<bits/socket.h>
struct sockaddr_storage{
    sa_family_t sa_family;//sa_family_t 地址族类型
    unsigned long int __ss_align;
    char __ss_padding[128-sizeof(__ss_align)];//内存对齐
};
```
协议族和地址族关系
协议族|地址族|描述
-|-|-
PF_UNIX|AF_UNIX|UNIX本地域协议族
PF_INET|AF_INET|TCP/IPv4协议族
PF_INET6|AF_INET6|TCP/IPv6协议族

## 专用socket地址
* unix本地域协议簇
```
#include<sys/un.h>
struct sockaddr_un{
    sa_family_t sin_family;
    char sun_path[108];
};
```

* IPv4
```
struct sockaddr_in{
    sa_family_t sin_family;
    u_int16_t sin_port;
    struct in_addr sin_addr;
};

struct in_addr{
    u_int32_t s_addr;
};
```


*IPv6
```
struct sockaddr_in6{
    sa_family_t sin6_family;//地址簇
    u_int16_t sin6_port;//端口号
    struct in6_addr sin6_addr;//
    u_int32_t sin6_scope_id;//scope ID,尚处于实验阶段
};

struct in6_addr{
    unsigned char sa_addr[16];
};
```

## IP地址转换函数
#include<arpa/inet.h>

## 创建socket
```
#include<sys/types.h>
#include<sys/socket.h>
int socket(int domain,int type,int protocol);
//domain:PF_INET PF_INET6 PF_UNIX
//type:服务类型 SOCK_STREAM流服务 SOCK_UGRAM数据报
//protocol:通常为0,
```

## 命名socket
```
#include<sys/types.h>
#include<sys/socket.h>
//将my_addr所指的地址分配给sockfd文件描述符,addrlen参数指出该socket地址的长度
int bind(int socket,const struct sockaddr* my_addr,socklen_t addrlen);
```

## 监听socket
socket命名后,还不能接受客户连接,还需要放入监听队列以存放待处理的客户连接

这个监听队列只有一个,是放在内核中的
```
#include<sys/socket.h>
int listen(int sockfd,int backlog);
//backlog 监听队列的最大长度
```

## 接受连接
```
#include<sys/types.h>
#include<sys.socket.h>
int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
//连接成功会返回新的socket
//addr,远端服务器socket
//addrlen,远端服务器socket长度
```

## 发起连接
```
//主动连接
#include<sys/types.h>
#include<sys/socket.h>
int connect(int sockfd,const struct sockaddr *serv_addr,socklen_t addrlen);
```

## 关闭连接
```
#include<unistd.h>
int close(int fd);
```

# 数据读写
## TCP数据读写
