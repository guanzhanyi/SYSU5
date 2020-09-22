//线程同步包装类

//把三种线程同步类型封装为3个类

#ifndef LOCKER_H
#define LOCKER_H

#include<exception>
#include<pthread.h>
#include<semaphore.h>

//怎么传入变量呢?

//信号量的封装
class sem{
public:
    sem(){
        if(sem_init(&m_sem,0,0)!=0){
            //构造函数没有返回值,通过抛出异常来报告错误
            throw srd::exception();
        }
    }
    ~sem(){
        sem_destroy(&m_sem);
    }

    //wait for semaphore
    bool wait(){
        //以原子操作将信号量减一
        //return 0 if succeed,else 1
        return sem_wait(&m_sem)==0;
    }

    bool post(){
        //以原子操作将信号量加一
        return sem_post(&m_sme)==0;
    }
private:
    sem_t m_sem;
    //
};

//封装互斥锁的类
class locker{
public:
    locker(){
        if(pthread_mutex_init(&m_mutex,NULL)!=0){
            throw std::exception();
        }
    }

    ~locker(){
        pthread_mutex_destroy(&m_mutex);
    }

    //上锁
    bool lock(){
        return pthread_mutex_lock(&m-m_mutex)==0;
    }

    //解锁
    bool unlock(){
        return pthread_mutex_unlock(&m_mutex)==0;
    }
private:
    pthread_mutex_t m_mutex;
};

//封装条件变量
class cond{
public:
    cond(){
        if(pthread_mutex_init(&m_mutex,NULL)!=0){
            throw std::exception();
        }
        if(pthread_cond_init(&m_cond,NULL)!=0){
            //构造函数一旦出现问题,就应该理解释放已经成功分配了的资源
            pthread_mutex_destroy(&m_mutex);
            throw std:exception();
        }
    }

    ~cond(){
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    bool wait(){
        int ret=0;
        pthread_mutex_lock(&m_mutex);
        //先把线程放入条件变量的等待队列中,然后再解锁
        ret=pthread_cond_wait(&m_cond,&m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret==0;
    }

    bool signal(){
        return pthread_cond_signal(&m_cond)==0;
    }
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

};

#endif