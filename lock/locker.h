#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>
#include <stdexcept>

class sem {
public:
    sem() {
        if (sem_init(&m_sem, 0, 0) != 0)  //初始化一个 unnamed semaphore，初始值为 0，表示没有可用资源。
            throw std::exception();
    }   
    sem(int num) {
        if (sem_init(&m_sem, 0, num) != 0)  //初始化一个 unnamed semaphore，初始值为 num，表示有 num 个可用资源。
            throw std::exception();
    }
    ~sem() {
        sem_destroy(&m_sem);   //销毁信号量，释放资源。
    }

    bool wait() {
        return sem_wait(&m_sem) == 0;  //等待信号量，当信号量的值大于 0 时，减 1 并继续执行；当信号量的值为 0 时，阻塞等待直到有资源可用。
    }
    bool post() {
        return sem_post(&m_sem) == 0;  //释放信号量，增加 1，表示有一个资源可用。如果有线程在等待该信号量，则唤醒其中一个线程。
    }
    sem(const sem&) = delete;
    sem& operator=(const sem&) = delete;
private:
    sem_t m_sem;  //信号量,用于控制访问共享资源的线程数量。当一个线程调用 wait() 时，信号量的计数会减少；当一个线程调用 post() 时，计数会增加。
};

class locker {
public:
    locker() {
        if (pthread_mutex_init(&m_mutex, NULL) != 0) {
            throw std::runtime_error("mutex init failed");
        }
    }
    ~locker() noexcept {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        if (pthread_mutex_lock(&m_mutex) != 0) {
            throw std::runtime_error("mutex lock failed");
        }
    }

    void unlock() {
        if (pthread_mutex_unlock(&m_mutex) != 0) {
            throw std::runtime_error("mutex unlock failed");
        }
    }
    
    pthread_mutex_t *get() {
        return &m_mutex;
    }

    locker(const locker&) = delete;
    locker& operator=(const locker&) = delete;
private:
    pthread_mutex_t m_mutex;
};

//条件变量就是实现这种“睡眠-唤醒”机制的工具，它必须配合互斥锁使用。
class cond { 
public:
    cond() {
        if (pthread_cond_init(&m_cond, NULL) != 0) {
            throw std::runtime_error("cond init failed");
        }
    }
    ~cond() {
        pthread_cond_destroy(&m_cond);
    }
    bool wait(pthread_mutex_t *m_mutex) {
        return pthread_cond_wait(&m_cond, m_mutex) == 0;
    }
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t) {
        return pthread_cond_timedwait(&m_cond, m_mutex, &t) == 0;
    }
    bool signal() {
        return pthread_cond_signal(&m_cond) == 0;
    }
    bool broadcast() {
        return pthread_cond_broadcast(&m_cond) == 0;
    }
    cond(const cond&) = delete;
    cond& operator = (const cond&) = delete;
private:
    pthread_cond_t m_cond;
};

#endif
