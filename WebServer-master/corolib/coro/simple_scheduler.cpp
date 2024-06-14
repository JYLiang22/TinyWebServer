/*
 * @file simple_scheduler.cpp
 * @brief 简单的协程调度器实现
 * @date 2024.6.14
 * @cite https://www.midlane.top/wiki/pages/viewpage.action?pageId=10060963
*/

#include<iostream>
#include<list>
#include"fiber.h"

/*
 * @brief 简单协程调度类，支持添加调度任务和运行调度任务
*/
class Scheduler{
private:
    /// 任务队列
    std::list<coro::Fiber::ptr> m_tasks;

public:
    /*
     * @brief 添加协程调度任务
    */
    void schedule(coro::Fiber::ptr task){
        m_tasks.push_back(task);
    }

    /*
     * @brief 执行协程调度任务
    */
    void run(){
        coro::Fiber::ptr task;
        auto it = m_tasks.begin();
        while(it != m_tasks.end()){
            task = *it;
            m_tasks.erase(it++);
            task->resume();
        }
    }
};

void test_fiber(int i){
    std::cout << "hello " << i << std::endl;
}

int main(){

    /// 初始化当前线程的主协程
    coro::Fiber::GetThis();

    /// 创建调度器
    Scheduler sc;

    /// 添加调度任务
    std::cout << "add scheduler task" << std::endl;
    for(int i = 0; i < 10; ++i){
        coro::Fiber::ptr fiber(new coro::Fiber(std::bind(test_fiber, i)));
        std::cout << "add scheduler task " << i << std::endl;
        sc.schedule(fiber);
    }

    /// 执行调度任务
    sc.run();

    return 0;
}
