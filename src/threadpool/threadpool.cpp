#include "threadpool.h"
#include<assert.h>

explicit ThreadPool::ThreadPool(size_t threadCount = 8) : pool_(std::make_shared<Pool>()){
    assert(threadCount > 0);
    for(size_t i = 0; i < threadCount; ++i){
        std::thread([pool = pool_]{
            std::unique_lock<std::mutex> locker(pool->mtx);
            while(true){
                if(!pool->tasks.empty()){
                    auto task = std::move(pool->tasks.front());
                    pool->tasks.pop();
                    locker.unlock();
                    task();
                    locker.lock();
                }
                else if(pool->isClosed){
                    break;
                }
                else{
                    pool->cond.wait(locker);
                }
            }
        }).detach();
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