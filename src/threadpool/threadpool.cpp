/*
* @author   JYLiang
* @date     2024.5.14
*/

#include "threadpool.h"
#include<assert.h>

explicit ThreadPool::ThreadPool(size_t threadCount = 8) : pool_(std::make_shared<Pool>()){
    assert(threadCount > 0);
    // 创建特定数量的线程
    for(size_t i = 0; i < threadCount; ++i){
        // 使用lambda表达式
        std::thread([pool = pool_]{
            std::unique_lock<std::mutex> locker(pool->mtx);
            // 执行任务
            while(true){
                // 检查队列中是否有任务
                if(!pool->tasks.empty()){
                    auto task = std::move(pool->tasks.front());
                    pool->tasks.pop();
                    locker.unlock();
                    task();   // 执行任务
                    locker.lock();
                }
                // 线程池关闭则退出循环
                else if(pool->isClosed){
                    break;
                }
                // 无任务则等待
                else{
                    pool->cond.wait(locker);
                }
            }
        }).detach();   // 分离线程独立运行，执行完可自动回收资源
    }
} 

ThreadPool::~ThreadPool(){
    if(static_cast<bool>(pool_)){
        std::lock_guard<std::mutex> locker(pool_->mtx);
        pool_->isClosed = true;
    }
    pool_->cond.notify_all();
}

template<typename F>
void ThreadPool::addTask(F &&task){
    {
        std::lock_guard<std::mutex> locker(pool_->mtx);
        pool_->tasks.emplace(std::forward<F>(task));
    }
    pool_->cond.notify_one();
}