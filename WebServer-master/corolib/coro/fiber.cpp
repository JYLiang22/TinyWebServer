/*
 * @file fiber.cpp
 * @brief 协程实现
 * @date 2024.6.13
 * @cite https://github.com/zhongluqiang/sylar-from-scratch/blob/main/sylar/fiber.cc
*/

#include<atomic>
#include<assert.h>
#include"fiber.h"

namespace coro{

/// 全局静态变量，用于生成协程id
static std::atomic<uint64_t> s_fiber_id{0};
/// 全局静态变量，用于统计当前协程数
static std::atomic<uint64_t> s_fiber_count{0};

/// 线程局部变量，当前线程正在运行的协程
static thread_local Fiber *t_fiber = nullptr;
/// 线程局部变量，当前线程的主协程，切换到这个协程，就相当于切换到了主线程中运行，智能指针形式
static thread_local Fiber::ptr t_thread_fiber = nullptr;

/// 定义默认栈空间为128K
#define DEFAULT_STACK_SIZE (1024*128)

/*
 * @brief malloc栈内存分配器
*/
class MallocStackAllocator{
public:
    static void *Alloc(size_t size) { return malloc(size); }
    static void Dealloc(void *vp, size_t size) { return free(vp); }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId(){
    if(t_fiber){
        return t_fiber->m_id;
    }
    return 0;
}

Fiber::Fiber(){
    SetThis(this);
    m_state = RUNNING;

    if(getcontext(&m_ctx)){
        /// 先不用log日志模块，怎么方便怎么来
        printf("Fiber::Fiber() getcontext failed\n");
        return;
    }

    ++s_fiber_count;
    m_id = s_fiber_id++;
    /// 先不用log日志模块，怎么方便怎么来
    printf("Fiber::Fiber() main id = %lu\n", m_id);
}

void Fiber::SetThis(Fiber *f){
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis(){
    if(t_fiber){
        return t_fiber->shared_from_this();
    }
    
    Fiber::ptr main_fiber(new Fiber);
    assert(t_fiber == main_fiber.get());
    t_thread_fiber = main_fiber;
    return t_fiber->shared_from_this();
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool run_in_scheduler)
     : m_id(s_fiber_id++), m_cb(cb), m_runInScheduler(run_in_scheduler){
    ++s_fiber_count;
    m_stacksize = (stacksize ? stacksize : DEFAULT_STACK_SIZE);
    m_stack = StackAllocator::Alloc(m_stacksize);

    if(getcontext(&m_ctx)){
        /// 先不用log日志模块，怎么方便怎么来
        printf("Fiber::Fiber(param in) getcontext failed\n");
        return;
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);

    /// 先不用log日志模块，怎么方便怎么来
    printf("Fiber::Fiber(param in) id = %lu\n", m_id);
}

/*
 * 线程的主协程析构时需要特殊处理，因为主协程没有分配栈和cb
*/
Fiber::~Fiber(){
    printf("Fiber::~Fiber() deconstruct id = %lu\n", m_id);
    --s_fiber_count;
    if(m_stack){
        // 有栈，说明这是子协程，需要确保子协程是结束状态才可以析构
        assert(m_state == TERM);
        StackAllocator::Dealloc(m_stack, m_stacksize);
        printf("Fiber::~Fiber() dealloc stack, id = %lu\n", m_id);
    }
    else{
        // 没有栈，说明是线程的主协程
        assert(!m_cb && m_state == RUNNING);   // 没有cb和处于运行状态
        Fiber *cur = t_fiber;
        if(cur == this){
            SetThis(nullptr);
        }
    }
}

/*
 * 为了简化状态，强制只有TERM状态的协程才可以重置，也就是只有子协程才可以重置
 * 重置协程就是重复利用已结束的协程，复用其栈空间，创建新协程
*/
void Fiber::reset(std::function<void()> cb){
    assert(m_cb && m_state == TERM);   // m_cb确保协程有栈，即协程是子协程
    m_cb = cb;
    if(getcontext(&m_ctx)){
        printf("Fiber::reset(param in) getcontext failed\n");
        return;
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = READY;
}

void Fiber::resume(){
    assert(m_state == READY);
    SetThis(this);
    m_state = RUNNING;

    // if(swapcontext(&(t_thread_fiber->m_ctx), &m_ctx)){
    //     printf("Fiber::resume() swapcontext failed\n");
    //     return;
    // }

    // 如果协程参与调度器调度，那么应该和调度器的主协程进行swap，而不是线程主协程
    if(m_runInScheduler){
        if(swapcontext(&(Scheduler::GetMainFiber()->m_ctx), &m_ctx)){
            printf("Fiber::resume() runInScheduler swapcontext failed\n");
            return;
        }
    }
    else{
        if(swapcontext(&(t_thread_fiber->m_ctx), &m_ctx)){
            printf("Fiber::resume() not runInScheduler swapcontext failed\n");
            return;
        }
    }
}

void Fiber::yield(){
    assert(m_state == RUNNING || m_state == TERM);
    SetThis(t_thread_fiber.get());
    if(m_state != TERM){
        m_state == READY;
    }

    // if(swapcontext(&m_ctx, &(t_thread_fiber->m_ctx))){
    //     printf("Fiber::yield() swapcontext failed\n");
    //     return;
    // }
    // 如果协程参与调度器调度，那么应该和调度器的主协程进行swap，而不是线程主协程
    if(m_runInScheduler){
        if(swapcontext(&m_ctx, &(Scheduler::GetMainFiber()->m_ctx))){
            printf("Fiber::yield() runInScheduler swapcontext failed\n");
            return;
        }
    }
    else{
        if(swapcontext(&m_ctx, &(t_thread_fiber->m_ctx))){
            printf("Fiber::yield() not runInScheduler swapcontext failed\n");
            return;
        }
    }
}

void Fiber::MainFunc(){
    Fiber::ptr cur = GetThis();
    assert(cur);

    cur->m_cb();
    cur->m_cb = nullptr;
    cur->m_state = TERM;

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->yield();
}

}   // namespace coro
