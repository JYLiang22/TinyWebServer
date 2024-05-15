/*
* @author   JYLiang
* @date     2024.5.14
*/

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include<iostream>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<thread>
#include<functional>

class ThreadPool{
private:
    // 使用结构体保存线程池内部状态
    struct Pool{
        std::mutex mtx;                             // 互斥量
        std::condition_variable cond;               // 条件变量
        bool isClosed;                              // 线程池是否已停止运行
        std::queue<std::function<void()>> tasks;    // 任务队列
    };

    std::shared_ptr<Pool> pool_;                    // 使用智能指针管理线程池结构体

public:
    // 初始化线程池，默认大小为8
    explicit ThreadPool(size_t threadCount);

    // 构造函数和移动构造函数定义为默认构造，由编译器实现
    ThreadPool() = default;
    ThreadPool(ThreadPool &&) = default;

    // 析构函数
    ~ThreadPool();

    // 往线程池中添加任务
    template<typename F>
    void addTask(F &&task);
};

#endif   