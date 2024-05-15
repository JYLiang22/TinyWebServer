/*
* @author   JYLiang
* @date     2024.5.14
*/

#include "threadpool.h"
#include<assert.h>

explicit ThreadPool::ThreadPool(size_t threadCount = 8) : pool_(std::make_shared<Pool>()){
    assert(threadCount > 0);
    // �����ض��������߳�
    for(size_t i = 0; i < threadCount; ++i){
        // ʹ��lambda���ʽ
        std::thread([pool = pool_]{
            std::unique_lock<std::mutex> locker(pool->mtx);
            // ִ������
            while(true){
                // ���������Ƿ�������
                if(!pool->tasks.empty()){
                    auto task = std::move(pool->tasks.front());
                    pool->tasks.pop();
                    locker.unlock();
                    task();   // ִ������
                    locker.lock();
                }
                // �̳߳عر����˳�ѭ��
                else if(pool->isClosed){
                    break;
                }
                // ��������ȴ�
                else{
                    pool->cond.wait(locker);
                }
            }
        }).detach();   // �����̶߳������У�ִ������Զ�������Դ
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