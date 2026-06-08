#include <queue>
#include <iostream>
#include <unistd.h>
#include "./locker.h"

using namespace std;

std::queue<int> q;
locker mutex;
cond cv;
bool stop = false;  // 用于通知消费者线程停止

// 生产者线程
void *producer(void *arg) {
    for (int i = 0; i < 10; ++i) {
        mutex.lock();
        q.push(i);
        std::cout << "produced " << i << std::endl;
        cv.signal();   // 唤醒一个消费者（如果有在等）
        mutex.unlock();
        sleep(1);  // 模拟生产时间
    }
    mutex.lock();
    stop = true;
    cv.broadcast();
    mutex.unlock();
    return nullptr;
}

// 消费者线程
void *consumer(void *arg) {
    while (true) {
        mutex.lock();
        while (q.empty() && !stop) {        // 必须用 while，防止虚假唤醒
            cv.wait(mutex.get());  // 队列空，睡眠等待
        }
        if (stop && q.empty()) {
            mutex.unlock();
            break;
        }
        int val = q.front();
        q.pop();
        mutex.unlock();
        std::cout << "consumed " << val << std::endl;
    }
    return nullptr;
}

int main() {
    pthread_t prod, cons;   // 定义两个线程 ID 变量
    pthread_create(&prod, nullptr, producer, nullptr);  // 启动生产者线程
    pthread_create(&cons, nullptr, consumer, nullptr);  // 启动消费者线程
    pthread_join(prod, nullptr);   // 等待生产者结束
    pthread_join(cons, nullptr);   // 等待消费者结束
    std::cout << "Test finished." << std::endl;
    return 0;
}