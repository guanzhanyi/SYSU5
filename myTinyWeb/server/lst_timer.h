/*
定时器是干什么的呢?指定一段时间后触发某段代码执行
定时器的作用非常大,如果某个长连接长久没有操作,那么定时器检测到该长连接的状态后,会提醒处理代码去处理
定时器往往和SIGALRM信号连用
***************************************
定时器的实现思路是什么呢?
这是一个针对网络编程的定时器
定时器包括超时时间,任务回调函数,,任务回调函数的参数,和指向下一个定时器的指针

*/
//基于升序链表的定时器
#ifndef LST_TIMER
#define LST_TIMER

#include<time.h>
#define BUFFER_SIZE 64;

class util_timer;

//用户数据结构,客户端socket地址,socket文件描述符,读缓存,定时器等
struct client_data{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer* timer;
};

//定时器类
class util_timer{
public:
    util_timer():prev(NULL),next(NULL){}

public:
    time_t expire;//任务的超时时间,这里使用绝对时间
    void (*cb_func)(client_data*);//Callback function
    client_data* user_data;
    util_timer* prev;//指向前一个计时器
    util_timer* next;//指向下一个计时器
};

//定时器链表,升序,双向链表
class sort_timer_lst{
public:
    sort_timer_lst():head(NULL),tail(NULL){}
    //销毁所有链表,删除其中所有的定时器
    ~sort_timer_lst(){
        util_timer* tmp=head;
        while(tmp){
            head=tmp->next;
            delete tmp;
            tmp=head;
        }
    }
    
    //将目标定时器timer添加到链表中
    void _add_timer(util_timer *timer){
        //如果定时器为空,返回
        if(!timer){
            return ;
        }
        //如果定时器链表长度为0,初始化链表
        if(!head){
            head=tail=timer;
            return ;
        }
        //add_timer()函数没有 把适当的timer插入链表head的功能,所以这里检测了一下是否应该为头部
        //事实上我不知道这样子是否好,完全可以让add_timer具有这个功能的
        if(timer->expire<head->expire){
            timer->next=head;
            head->prev=timer;
            head=timer;
            return;
        }
        _add_timer(timer,head);
    }

    //当timer的expire改变时调整定时器
    void adjust_timer(util_timer *timer){
        if(!timer){
            return;
        }
        //如果该定时器改变的超时时间仍符合所处位置,则无需改变
        //位于头部,比较next
        //位于尾部,比较prev
        //位于中间,两端比较
        if((timer->expire>timer->prev->expire||timer->prev==NULL)&&(timer->next==NULL||timer->next->expire>timer->expire)){
            return;
        }
        //否则,取出timer,重新插入
        else{
            _del_timer(timer);
            _add_timer(timer,head);
            return;
        }
    }

    //删除目标定时器
    void del_timer(util_timer *timer){
        if(!timer){
            return;
        }
        if((timer==head)&&(timer==tail)){
            delete timer;
            head=NULL;
            tail==NULL;
            return;
        }
        else if(timer==head&&timer!=tail){
            head=timer->next;
            delete timer;
            return;
        }
        else if(timer==tail&&timer!=head){
            tail=timer->prev;
            delelte timer;
            return;
        }
        else{
            _del_timer(timer);
            delete timer;
            return;
        }
    }

    //SIGALRM信号每次触发就在其信号处理函数中执行一次tick()函数,已处理链表上到期的任务
    void tick(){
        if(!head){
            return;
        }
        printf("timer tick\n");
        //获取系统当前时间
        time_t cur=time(NULL);
        util_timer* tmp=head;
        //从头到尾依次处理每一个定时器,直到遇到一个尚未到期的定时器
        while(tmp){
            //?
            if(cur<tmp->expire){
                break;
            }
            //调用定时器的回调函数,以执行定时任务?
            tmp->cb_func(tmp->user_data);
            //执行完定时器的定时任务后,将其删除,重置链表头节点
            head=tmp->next;
            if(head){
                head->prev=NULL;
            }
            delete tmp;
            tmp=head;
        }
    }
private:
    //将目标定时器timer添加到节点lst_head之后的部分链表中
    void _del_timer(util_timer *timer){
        timer->prev->next=timer->next;
        timer->next->prev=timer->prev;
    }
    void _add_timer(util_timer *timer, util_timer* lst_head){
        util_timer* prev = lst_head;
        util_timer* tmp=prev->next;
        //遍历lst_head节点之后的部分链表,直到找到一个超时时间大于目标定时器的超时时间节点,并将目标定时器插入该节点
        while(tmp){
            if(timer->expire<tmp->expire){
                prev->next=timer;
                timer->next=tmp;
                tmp->prev=timer;
                timer->prev=prev;
                break;
            }
            prev=tmp;
            tmp=tmp->next;
        }
        //如果遍历完lst_timer后仍然找不到超时时间大于目标定时器的超时时间的节点,则把timer插入尾部
        if(!tmp){
            prev->next=timer;
            timer->prev=prev;
            timer->next=NULL;
            tail=timer;
        }
    }
private:
    util_timer* head;
    util_timer* tail;
}


//处理非活动连接,统一事件源
//就是函数主循环和信号处理函数是两条不一样的执行路线
//在信号处理过程中,主循环会对信号进行蒙蔽,即掩码.如果信号处理太久,主循环就会长久的不能接收到该信号,这并不是个很好的信号处理方式
//同一事件源 则把信号处理的主要逻辑放在主函数上处理,而信号处理函数被激发时,将通过管道通知主函数

class Utils{
public:
    Utils(){}
    ~Utils(){}

    void init(int timeslot){
        m_TIMESLOT=timeslot;
    }

    //对文件描述符设置非阻塞
    //非阻塞意味着如果读不到什么或者写满了会立即返回错误,不被阻塞
    //有两种方法指定一个描述符非阻塞I/O
    //调用open时指定或已经打开的调用fcntl
    int setnonblocking(int fd){
        int old_option=fcntl(fd, F_GETFL);
        int new_option=old_optioin|O_NONBLOCK;
        fcnlt(fd,F_SETFL,new_option);
        //为什么要返回old_option
        return old_option;
    }

    //将内核事件表注册读时事件,ET模式,选择开启EPOLLONESHOT(防止一个ET事件被多次读.此时操作系统最多只会触发其上注册的一个可读,可写或者异常事件)
    void addfd(int epolled,int fd,bool one_shot,int TRIGMode){
        epoll_event event;
        event.data.fd = fd;
        if(TRIGMode==1){
            //EPOLLRDHUP    
            event.events=EPOLLIN| EPOLLET|EPOLLRDHUP;
        }
        else{
            event.events=EPOLLIN| EPOLLRDHUP;
        }
        if(one_shot){
            event.events|=EPOLLONESHOT;
        }
        epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
        setnonblocking(fd);
    }

    //信号处理函数
    static void sig_handler(int sig){
        int save_errno;
        int msg=sig;
        //msg不是int吗,怎么send
        send(u_pipefd[1],(char*)&msg,1,0);
        errno = save_errno;
    }

    //设置信号函数
    void addsig( int sig, void (handler)(int),bool restart){
        struct sigaction sa;
        memset(&sa,'\0',sizeof(sa));
        sa.sa_handler=handler;
        if(restart)
            //?
            sa.sa_flags|=SA_RESTART;
        sigfillset(&sa.sa_mask);
        //assert 
        assert(sigaction(sig,&sa,NULL)!=-1);
    }

    //定时处理任务,重新定时以不断触发SIGALRM信号
    void timer_handler(){
        m_timer_lst.tick();
        //?
        alarm(m_TIMESLOT);
    }

    //?
    void show_error(int connfd,const char *info){
        send(connfd,info,strlen(info),0);
        close(connfd);
    }
public:
    static int *u_pipefd;
    sort_timer_lst m_timer_lst;
    static int u_epollfd;
    int m_TIMESLOT;
};

int *Utils::u_pipefd=0;
int Utils::u_epollfd=0;

class Utils;

void cb_func(client_data *user_data){
    epoll_event(Utils::u_epollfd,EPOLL_CTL_DEL,user_data->sockfd,0);
    assert(user_data);
    close(user_data->sockfd);
    http_conn::m_user_count-=1;
}

