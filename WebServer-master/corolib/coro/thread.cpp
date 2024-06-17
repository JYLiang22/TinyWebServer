/*
 * @file thread.cpp
 * @brief 线程封装实现
 * @date 2024.6.17
 * @cite https://github.com/zhongluqiang/sylar-from-scratch/blob/main/sylar/thread.cc
*/

#include<iostream>
#include"thread.h"
#include"util.h"

namespace coro{

/// 当前线程的Thread指针
static thread_local Thread *t_thread = nullptr;
/// 当前线程名称
static thread_local std::string t_thread_name = "UNKNOW";

Thread* Thread::GetThis(){
    return t_thread;
}

const std::string& Thread::GetName(){
    return t_thread_name;
}

void Thread::SetName(const std::string &name){
    if(name.empty()){
        return;
    }
    if(t_thread){
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string &name) : m_cb(cb), m_name(name){
    if(name.empty()){
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(rt){
        std::cout << "pthread_create thread fail, rt = "<< rt << " name = " << name << std::endl;
        return;
    }
    m_semaphore.wait();
}

Thread::~Thread(){
    if(m_thread){
        pthread_detach(m_thread);
    }
}

void Thread::join(){
    if(m_thread){
        int rt = pthread_join(m_thread, nullptr);
        if(rt){
            std::cout << "pthread_join thread fail, rt = " << rt << " name = " << m_name << std::endl;
            return;
        }
        m_thread = 0;
    }
}

void* Thread::run(void *arg){
    Thread *thread = (Thread *)arg;
    // std::cout << thread << std::endl;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = coro::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);

    thread->m_semaphore.notify();
    return 0;
}

}

