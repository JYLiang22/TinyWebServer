/*
 * @file fiber_test1.cpp
 * @brief 协程测试，用于演示非对称协程
 * @date 2024.6.13
 * @cite https://github.com/zhongluqiang/sylar-from-scratch/blob/main/tests/test_fiber2.cc
*/

#include<string>
#include<vector>
#include"../coro/fiber.h"

void runinFiber2(){
    printf("run in fiber2 begin\n");
    printf("run in fiber2 end\n");
}

void runinFiber1(){
    printf("run in fiber1 begin\n");
    // coro::Fiber::ptr fiber(new coro::Fiber(runinFiber2));
    // fiber->resume();
    printf("run in fiber1 end\n");
}

int main(int argc, char *argv[]){

    printf("main begin\n");

    coro::Fiber::GetThis();
    printf("Main GetFiberId : %lu\n", coro::Fiber::GetFiberId());
    printf("Main getId : %lu\n", coro::Fiber::GetThis()->getId());

    coro::Fiber::ptr fiber1(new coro::Fiber(runinFiber1));
    printf("runinFiber1 GetFiberId : %lu\n", fiber1->GetFiberId());
    printf("runinFiber1 getId : %lu\n", fiber1->getId());
    fiber1->resume();

    coro::Fiber::ptr fiber2(new coro::Fiber(runinFiber2));
    printf("runinFiber2 GetFiberId : %lu\n", fiber2->GetFiberId());
    printf("runinFiber2 getId : %lu\n", fiber2->getId());
    fiber2->resume();

    printf("main end\n");

    return 0;
}
