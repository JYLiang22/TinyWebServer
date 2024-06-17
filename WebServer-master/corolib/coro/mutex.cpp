/*
 * @file mutex.cpp
 * @brief 信号量实现
 * @date 2024.6.17
 * @cite https://github.com/zhongluqiang/sylar-from-scratch/blob/main/sylar/mutex.cc
*/

#include"mutex.h"

namespace coro{

Semaphore::Semaphore(uint32_t count){
    if(sem_init(&m_semaphore, 0, count)){
        printf("sem_init error\n");
        return;
    }
}

Semaphore::~Semaphore(){
    sem_destroy(&m_semaphore);
}

void Semaphore::wait(){
    if(sem_wait(&m_semaphore)){
        printf("sem_wait error\n");
        return;
    }
}

void Semaphore::notify(){
    if(sem_post(&m_semaphore)){
        printf("sem_post error\n");
        return;
    }
}

}
