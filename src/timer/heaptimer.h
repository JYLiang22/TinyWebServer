/*
* @Author : JYLiang
* @Date : 2024.05.29
*/

#ifndef _HEAPTIMER_H_
#define _HEAPTIMER_H_

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h>
#include <functional>
#include <assert.h>
#include <chrono>
#include "../log/log.h"

// Define a callback function type for timeout events
typedef std::function<void()> TimeoutCallBack;

// Define aliases for time management
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

// Structure to represent each timer node
struct TimerNode {
    int id; // Unique identifier for the timer
    TimeStamp expires; // Expiration time of the timer
    TimeoutCallBack cb; // Callback function to be executed when timer expires

    // Overload the less-than operator to compare timer nodes based on expiration time
    bool operator<(const TimerNode &t) {
        return expires < t.expires;
    }
};

class HeapTimer {
private:
    // Helper function to delete a timer node by index
    void del(size_t index);
    // Helper function to maintain heap property by moving a node up
    void siftup(size_t index);
    // Helper function to maintain heap property by moving a node down
    bool siftdown(size_t index, size_t n);
    // Helper function to swap two nodes in the heap
    void swapNode(size_t index, size_t j);

    // Vector to store the heap of timer nodes
    std::vector<TimerNode> heap;
    // Hash map to quickly find the index of a timer node by its id
    std::unordered_map<int, size_t> ref;

public:
    // Constructor to initialize the heap timer
    HeapTimer();
    // Destructor to clear the heap timer
    ~HeapTimer();

    // Adjust the expiration time of a timer
    void adjust(int id, int newExpires);
    // Add a new timer or update an existing timer
    void add(int id, int timeOut, const TimeoutCallBack &cb);
    // Execute the callback function of a timer and remove it from the heap
    void doWork(int id);
    // Clear all timers
    void clear();
    // Check and execute expired timers
    void tick();
    // Remove the top (minimum) timer node from the heap
    void pop();
    // Get the time until the next timer expires
    int getNextTick();
};

#endif
