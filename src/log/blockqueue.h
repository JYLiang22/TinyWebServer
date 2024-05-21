/*
* @author JYLiang
* @date 2024.5.21
*/

#ifndef _BLOCKQUEUE_H_
#define _BLOCKQUEUE_H_

#include<mutex>
#include<deque>
#include<condition_variable>
#include<sys/time.h>

template<typename T>
class BlockQueue{
private:
    std::deque<T> deq;
    size_t capacity;
    std::mutex mtx;
    bool isClose;
    std::condition_variable conConsumer;
    std::condition_variable conProducer;

public:
    explicit BlockQueue(size_t maxCapacity = 1000);
    ~BlockQueue();

    void clear();
    bool empty();
    bool full();
    void close();

    size_t size();
    size_t capacity();

    T front();
    T back();

    void push_back(const T &item);
    void push_front(const T &item);

    bool pop(T &item);
    bool pop(T &item, int timeout);

    void flush();
};

#endif