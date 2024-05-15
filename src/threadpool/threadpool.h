/*
* @author   JYLiang
* @date     2024.5.14
*/

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include<iostream>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<thread>
#include<functional>

class ThreadPool{
private:
    // ʹ�ýṹ�屣���̳߳��ڲ�״̬
    struct Pool{
        std::mutex mtx;                             // ������
        std::condition_variable cond;               // ��������
        bool isClosed;                              // �̳߳��Ƿ���ֹͣ����
        std::queue<std::function<void()>> tasks;    // �������
    };

    std::shared_ptr<Pool> pool_;                    // ʹ������ָ������̳߳ؽṹ��

public:
    // ��ʼ���̳߳أ�Ĭ�ϴ�СΪ8
    explicit ThreadPool(size_t threadCount);

    // ���캯�����ƶ����캯������ΪĬ�Ϲ��죬�ɱ�����ʵ��
    ThreadPool() = default;
    ThreadPool(ThreadPool &&) = default;

    // ��������
    ~ThreadPool();

    // ���̳߳����������
    template<typename F>
    void addTask(F &&task);
};

#endif   