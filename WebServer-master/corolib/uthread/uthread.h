#ifndef _UTHREAD_H_
#define _UTHREAD_H_

#include<ucontext.h>
#include<vector>

#define DEFAULT_STACK_SIZE (1024*128)
#define MAX_UTHREAD_SIZE 1024

// 协程的运行状态，包括空闲 就绪 正在执行 挂起
enum ThreadState{
    FREE,
    RUNNABLE,
    RUNNING,
    SUSPEND
};

typedef void (*Fun)(void *arg);

// 协程结构体
typedef struct uthread_t{
    ucontext_t ctx;   // 保存协程的上下文
    Fun func;   // 协程执行的用户函数
    void *arg;   // func的参数
    enum ThreadState state;   // 协程的运行状态
    char stack[DEFAULT_STACK_SIZE];   // 协程的栈
}uthread_t;

// 协程调度器结构体
typedef struct schedule_t{
    ucontext_t main;   // 主函数的上下文
    int running_thread;   // 正在执行的协程的编号
    uthread_t *threads;   // 调度器拥有的所有协程
    int max_index;   // 协程最大数量

    schedule_t() : running_thread(-1), max_index(0){
        threads = new uthread_t[MAX_UTHREAD_SIZE];
        for(int i = 0; i < MAX_UTHREAD_SIZE; ++i){
            threads[i].state = FREE;
        }
    }

    ~schedule_t(){
        delete []threads;
    }
}schedule_t;

// 协助协程在调度器中运行
static void uthread_body(schedule_t *ps);
// 创建一个协程，该协程的会加入到schedule的协程序列中，func为其执行的函数，arg为func的执行函数。返回创建的线程在schedule中的编号
int uthread_create(schedule_t &schedule, Fun func, void *arg);
// 挂起调度器schedule中当前正在执行的协程，切换到主函数
void uthread_yield(schedule_t &schedule);
// 恢复运行调度器schedule中编号为id的协程
void uthread_resume(schedule_t &schedule, int id);
// 判断schedule中所有的协程是否都执行完毕，是返回1，否则返回0
int schedule_finished(const schedule_t &schedule);

#endif
