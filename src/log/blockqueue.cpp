/*
* @author JYLiang
* @date 2024.5.21
*/

#include"./blockqueue.h"

template<typename T>
BlockQueue<T>::BlockQueue(size_t maxCapacity) : capacity(maxCapacity){
    assert(maxCapacity > 0);
    isClose = false;
}

template<typename T>
BlockQueue<T>::~BlockQueue(){
    close();
}

template<typename T>
void BlockQueue<T>::close(){
    {
        std::lock_guard<std::mutex> locker(mtx);
        deq.clear();
        isClose = true;
    }
    conProducer.notify_all();
    conConsumer.notify_all();
}

template<typename T>
void BlockQueue<T>::flush(){
    conConsumer.notify_one();
}

template<typename T>
void BlockQueue<T>::clear(){
    std::lock_guard<std::mutex> locker(mtx);
    deq.clear();
}

template<typename T>
T BlockQueue<T>::front(){
    std::lock_guard<std::mutex> locker(mtx);
    return deq.front();
}

template<typename T>
T BlockQueue<T>::back(){
    std::lock_guard<std::mutex> locker(mtx);
    return deq.back();
}

template<typename T>
size_t BlockQueue<T>::size(){
    std::lock_guard<std::mutex> locker(mtx);
    return deq.size();
}

template<typename T>
size_t BlockQueue<T>::capacity(){
    std::lock_guard<std::mutex> locker(mtx);
    return capacity;
}

template<typename T>
void BlockQueue<T>::push_back(const T &item){
    std::unique_lock<std::mutex> locker(mtx);
    while(deq.size() >= capacity){
        conProducer.wait(locker);
    }
    deq.push_back(item);
    conConsumer.notify_one();
}

template<typename T>
void BlockQueue<T>::push_front(const T &item){
    std::unique_lock<std::mutex> locker(mtx);
    while(deq.size() >= capacity){
        conProducer.wait(locker);
    }
    deq.push_front(item);
    conConsumer.notify_one();
}

template<typename T>
bool BlockQueue<T>::empty(){
    std::lock_guard<std::mutex> locker(mtx);
    return deq.empty();
}

template<typename T>
bool BlockQueue<T>::full(){
    std::lock_guard<std::mutex> locker(mtx);
    return deq.size() >= capacity;
}

template<typename T>
bool BlockQueue<T>::pop(T &item){
    std::unique_lock<std::mutex> locker(mtx);
    while(deq.empty()){
        conConsumer.wait(locker);
        if(isClose){
            return false;
        }
    }
    item = deq.front();
    deq.pop_front();
    conProducer.notify_one();
    return true;
}

template<typename T>
bool BlockQueue<T>::pop(T &item, int timeout){
    std::unique_lock<std::mutex> locker(mtx);
    while(deq.empty()){
        if(conConsumer.wait_for(locker, std::chrono::seconds(timeout)) == std::cv_status::timeout){
            return false;
        }
        if(isClose){
            return false;
        }
    }
    item = deq.front();
    deq.pop_front();
    conProducer.notify_one();
    return true;
}
