/*
 * @file thread_test.cpp
 * @brief 线程模块测试
 * @date 2024.6.17
 * @cite https://github.com/zhongluqiang/sylar-from-scratch/blob/main/tests/test_thread.cc
*/

#include<iostream>
#include"../coro/thread.h"
#include"../coro/util.h"

int count = 0;
coro::Mutex s_mutex;

void func1(void *arg){
    std::cout << "name: " << coro::Thread::GetName() 
    << " this.name: " << coro::Thread::GetThis()->GetName()
    << " thread name: " << coro::GetThreadName()
    << " id: " << coro::GetThreadId()
    << " this.id: " << coro::Thread::GetThis()->getID() << std::endl;
    std::cout << "arg: " << *(int *)arg << std::endl;
    for(int i = 0; i < 10000; ++i){
        coro::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

int main(int argc, char *argv[]){

    std::vector<coro::Thread::ptr> thrs;
    int arg = 123456;
    for(int i = 0; i < 3; ++i){
        // 带参数的线程用std::bind进行参数绑定
        coro::Thread::ptr thr(new coro::Thread(std::bind(func1, &arg), "thread_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    for(int i = 0; i < 3; ++i){
        thrs[i]->join();
        // std::bind(coro::Thread::GetThis()->run, thrs[i]);
    }

    std::cout << "count = " << count << std::endl;
    return 0;
}
