#include "./blockqueue.h"

// Constructor: initializes the queue with a specified capacity
template<typename T>
BlockQueue<T>::BlockQueue(size_t maxCapacity) : capacity(maxCapacity) {
    assert(maxCapacity > 0);
    isClose = false;
}

// Destructor: ensures the queue is closed and resources are freed
template<typename T>
BlockQueue<T>::~BlockQueue() {
    close();
}

// Closes the queue and notifies all waiting threads
template<typename T>
void BlockQueue<T>::close() {
    {
        std::lock_guard<std::mutex> locker(mtx);
        deq.clear();
        isClose = true;
    }
    conProducer.notify_all();
    conConsumer.notify_all();
}

// Notifies one waiting consumer
template<typename T>
void BlockQueue<T>::flush() {
    conConsumer.notify_one();
}

// Clears the queue
template<typename T>
void BlockQueue<T>::clear() {
    std::lock_guard<std::mutex> locker(mtx);
    deq.clear();
}

// Returns the front element of the queue
template<typename T>
T BlockQueue<T>::front() {
    std::lock_guard<std::mutex> locker(mtx);
    return deq.front();
}

// Returns the back element of the queue
template<typename T>
T BlockQueue<T>::back() {
    std::lock_guard<std::mutex> locker(mtx);
    return deq.back();
}

// Returns the current size of the queue
template<typename T>
size_t BlockQueue<T>::size() {
    std::lock_guard<std::mutex> locker(mtx);
    return deq.size();
}

// Returns the maximum capacity of the queue
template<typename T>
size_t BlockQueue<T>::capacity() {
    std::lock_guard<std::mutex> locker(mtx);
    return capacity;
}

// Adds an element to the back of the queue
template<typename T>
void BlockQueue<T>::push_back(const T &item) {
    std::unique_lock<std::mutex> locker(mtx);
    while (deq.size() >= capacity) {
        conProducer.wait(locker);
    }
    deq.push_back(item);
    conConsumer.notify_one();
}

// Adds an element to the front of the queue
template<typename T>
void BlockQueue<T>::push_front(const T &item) {
    std::unique_lock<std::mutex> locker(mtx);
    while (deq.size() >= capacity) {
        conProducer.wait(locker);
    }
    deq.push_front(item);
    conConsumer.notify_one();
}

// Checks if the queue is empty
template<typename T>
bool BlockQueue<T>::empty() {
    std::lock_guard<std::mutex> locker(mtx);
    return deq.empty();
}

// Checks if the queue is full
template<typename T>
bool BlockQueue<T>::full() {
    std::lock_guard<std::mutex> locker(mtx);
    return deq.size() >= capacity;
}

// Removes an element from the front of the queue
template<typename T>
bool BlockQueue<T>::pop(T &item) {
    std::unique_lock<std::mutex> locker(mtx);
    while (deq.empty()) {
        conConsumer.wait(locker);
        if (isClose) {
            return false;
        }
    }
    item = deq.front();
    deq.pop_front();
    conProducer.notify_one();
    return true;
}

// Removes an element from the front of the queue with a timeout
template<typename T>
bool BlockQueue<T>::pop(T &item, int timeout) {
    std::unique_lock<std::mutex> locker(mtx);
    while (deq.empty()) {
        if (conConsumer.wait_for(locker, std::chrono::seconds(timeout)) == std::cv_status::timeout) {
            return false;
        }
        if (isClose) {
            return false;
        }
    }
    item = deq.front();
    deq.pop_front();
    conProducer.notify_one();
    return true;
}
