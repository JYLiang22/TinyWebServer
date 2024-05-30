/*
* @Author : JYLiang
* @Date : 2024.05.29
*/

#include "heaptimer.h"

// Delete a timer node by its index in the heap
void HeapTimer::del(size_t index) {
    // Ensure the heap is not empty and the index is valid
    assert(!heap.empty() && index >= 0 && index < heap.size());
    size_t i = index; // The index to be deleted
    size_t n = heap.size() - 1; // The last index in the heap

    assert(i <= n); // Ensure the index is within bounds
    if (i < n) { // If the node is not the last one
        swapNode(i, n); // Swap it with the last node
        if (!siftdown(i, n)) { // Try to sift down the swapped node
            siftup(i); // If sifting down doesn't work, sift up instead
        }
    }
    ref.erase(heap.back().id); // Remove the node from the reference map
    heap.pop_back(); // Remove the last node (which was swapped)
}

// Move a node up in the heap to maintain the heap property
void HeapTimer::siftup(size_t index) {
    // Ensure the index is valid
    assert(index >= 0 && index < heap.size());
    size_t j = (index - 1) / 2; // Calculate the parent index
    while (j >= 0) { // While the node has a parent
        if (heap[j] < heap[index]) // If the parent node is smaller, the heap property is satisfied
            break;
        swapNode(index, j); // Otherwise, swap the node with its parent
        index = j; // Update the current index to the parent index
        j = (index - 1) / 2; // Recalculate the parent index
    }
}

// Move a node down in the heap to maintain the heap property
bool HeapTimer::siftdown(size_t index, size_t n) {
    // Ensure the index and bounds are valid
    assert(index >= 0 && index < heap.size());
    assert(n >= 0 && n <= heap.size());
    size_t i = index; // The starting index
    size_t j = i * 2 + 1; // The left child index
    while (j < n) { // While the node has at least one child
        if (j + 1 < n && heap[j + 1] < heap[j]) // If the right child is smaller than the left child
            ++j; // Move to the right child
        if (heap[i] < heap[j]) // If the current node is smaller than the child, the heap property is satisfied
            break;
        swapNode(i, j); // Otherwise, swap the node with the smaller child
        i = j; // Update the current index to the child index
        j = i * 2 + 1; // Recalculate the left child index
    }
    return i > index; // Return true if the node was moved
}

// Swap two nodes in the heap and update their indices in the reference map
void HeapTimer::swapNode(size_t index, size_t j) {
    // Ensure the indices are valid
    assert(index >= 0 && index < heap.size());
    assert(j >= 0 && j < heap.size());
    std::swap(heap[index], heap[j]); // Swap the nodes in the heap
    ref[heap[index].id] = index; // Update the reference map for the swapped nodes
    ref[heap[j].id] = j;
}

// Constructor to initialize the heap with a reserved size
HeapTimer::HeapTimer() {
    heap.reserve(64); // Reserve space for 64 timer nodes
}

// Destructor to clear the heap
HeapTimer::~HeapTimer() {
    clear(); // Clear all timers
}

// Adjust the expiration time of a timer
void HeapTimer::adjust(int id, int timeOut) {
    // Ensure the heap is not empty and the timer id exists
    assert(!heap.empty() && ref.count(id) > 0);
    heap[ref[id]].expires = Clock::now() + MS(timeOut); // Update the expiration time
    siftdown(ref[id], heap.size()); // Sift down to maintain the heap property
}

// Add a new timer or update an existing timer
void HeapTimer::add(int id, int timeOut, const TimeoutCallBack &cb) {
    // Ensure the timer id is valid
    assert(id >= 0);
    size_t index;
    if (ref.count(id) == 0) { // If the timer id does not exist
        index = heap.size(); // The new node will be added at the end
        ref[id] = index; // Update the reference map
        heap.push_back({id, Clock::now() + MS(timeOut), cb}); // Add the new timer node
        siftup(index); // Sift up to maintain the heap property
    } else { // If the timer id exists
        index = ref[id]; // Get the index of the existing timer
        heap[index].expires = Clock::now() + MS(timeOut); // Update the expiration time
        heap[index].cb = cb; // Update the callback function
        if (!siftdown(index, heap.size())) { // Try to sift down
            siftup(index); // If sifting down doesn't work, sift up instead
        }
    }
}

// Execute the callback function of a timer and remove it from the heap
void HeapTimer::doWork(int id) {
    if (heap.empty() || ref.count(id) == 0) { // If the heap is empty or the timer id does not exist
        return; // Do nothing
    }
    size_t index = ref[id]; // Get the index of the timer
    TimerNode node = heap[index]; // Get the timer node
    node.cb(); // Execute the callback function
    del(index); // Remove the timer from the heap
}

// Clear all timers
void HeapTimer::clear() {
    ref.clear(); // Clear the reference map
    heap.clear(); // Clear the heap
}

// Check and execute expired timers
void HeapTimer::tick() {
    if (heap.empty()) { // If the heap is empty
        return; // Do nothing
    }
    while (!heap.empty()) { // While there are timers in the heap
        TimerNode node = heap.front(); // Get the timer with the nearest expiration
        if (std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() > 0) {
            // If the nearest timer has not expired, break the loop
            break;
        }
        node.cb(); // Execute the callback function of the expired timer
        pop(); // Remove the expired timer from the heap
    }
}

// Remove the top (minimum) timer node from the heap
void HeapTimer::pop() {
    // Ensure the heap is not empty
    assert(!heap.empty());
    del(0); // Delete the root of the heap (minimum timer)
}

// Get the time until the next timer expires
int HeapTimer::getNextTick() {
    tick(); // Check and execute expired timers
    size_t res = -1; // Initialize the result
    if (!heap.empty()) {
        // If there are timers in the heap
        res = std::chrono::duration_cast<MS>(heap.front().expires - Clock::now()).count();
        // Get the time until the nearest timer expires
        if (res < 0) {
            // If the timer has already expired, set the result to 0
            res = 0;
        }
    }
    return res; // Return the time until the next timer expires
}
