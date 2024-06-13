/*
 * @file fiber.h
 * @brief 协程模块
 * @details 基于ucontext族函数实现非对称协程
 * @date 2024.6.12
 * @cite https://github.com/zhongluqiang/sylar-from-scratch/blob/main/sylar/fiber.h
*/

#ifndef _CORO_H_
#define _CORO_H_

#include<functional>
#include<memory>
#include<ucontext.h>

namespace coro{

/*
 * @brief 协程类
*/
class Fiber : public std::enable_shared_from_this<Fiber>{
public:
    typedef std::shared_ptr<Fiber> ptr;

    /*
     * @brief 协程状态
     * @details 只定义三种转换关系：正在运行 准备运行 结束运行
    */
    enum State{
        /// @brief 就绪态，刚创建或yield之后的状态
        READY,
        /// @brief 运行态，resume之后的状态
        RUNNING,
        /// @brief 结束态，协程回调函数执行完之后为TERM状态
        TERM
    };

private:
    /*
     * @brief 构造函数
     * @attention 无参构造函数只用于创建线程的第一个协程，也就是线程主函数对应的协程
     * 这个协程只能由GetThis()方法调用，所以设置成私有方法
    */
    Fiber();

public:
    /*
     * @brief 构造函数，用于创建用户协程
     * @param[in] cb 协程入口函数
     * @param[in] stacksize 栈大小
    */
    Fiber(std::function<void()> cb, size_t stacksize = 0);

    /*
     * @brief 析构函数
    */
    ~Fiber();

    /*
     * @brief 重置协程状态和入口函数，复用栈空间，不重新创建栈
     * @param[in] cb
    */
    void reset(std::function<void()> cb);

    /*
     * @brief 将当前协程切换到执行状态
     * @details 当前协程和正在运行运行的协程进行交换，前者变为RUNNING，后者变为READY
    */
    void resume();

    /*
     * @brief 当前协程让出执行权
     * @details 当前协程与上次resume退到后台的协程进行交换，前置状态变为READY，后者变为RUNNING
    */
    void yield();

    /*
     * @brief 获取协程ID
    */
    uint64_t getId() const { return m_id; }

    /*
     * @brief 获取协程状态
    */
    State getState() const { return m_state; }

public:
    /*
     * @brief 设置当前正在运行的协程，即设置线程局部变量t_fiber的值
    */
    static void SetThis(Fiber *f);

    /*
     * @brief 返回当前线程正在执行的协程
     * @details 如果当前线程未创建协程，则创建线程的第一个协程，且该协程为当前线程的主协程，其他协程都通过这个协程来调度
     * 也就是说，其他协程结束时候要回到主协程，由主协程重新选择新的协程进行resume
     * @attention 线程如果要创建协程，那么应该首先执行一下Fiber::GetThis()操作，以初始化主函数协程
    */
    static Fiber::ptr GetThis();

    /*
     * @brief 获取总协程数
    */
    static uint64_t TotalFibers();

    /*
     * @brief 协程入口函数
    */
    static void MainFunc();

    /*
     * @brief 获取当前协程id
    */
    static uint64_t GetFiberId();

private:
    /// 协程id
    uint64_t m_id = 0;
    /// 协程栈大小
    uint32_t m_stacksize = 0;
    /// 协程状态
    State m_state = READY;
    /// 协程上下文
    ucontext_t m_ctx;
    /// 协程栈地址
    void *m_stack = nullptr;
    /// 协程入口函数
    std::function<void()> m_cb;
};

}   // namespace coro

#endif