#ifndef BOUNDED_QUEUE_H
#define BOUNDED_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class BoundedQueue {
private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cv;
    size_t maxSize;
    bool finished;

public:
    BoundedQueue(size_t maxSize = 5) : maxSize(maxSize), finished(false) {}

    // Enqueue an item, blocks if queue is full
    bool enqueue(const T& item) {
        std::unique_lock<std::mutex> lock(mutex);
        
        // If queue is finished, don't accept new items
        if (finished) return false;
        
        // Wait until queue has space
        cv.wait(lock, [this]() { return queue.size() < maxSize || finished; });
        
        if (finished) return false;
        
        queue.push(item);
        cv.notify_all();
        return true;
    }

    // Try to dequeue an item, returns true if successful
    bool tryDequeue(T& item) {
        std::unique_lock<std::mutex> lock(mutex);
        
        if (queue.empty()) {
            return false;
        }
        
        item = queue.front();
        queue.pop();
        cv.notify_all();
        return true;
    }

    // Blocking dequeue
    bool dequeue(T& item) {
        std::unique_lock<std::mutex> lock(mutex);
        
        // Wait for data or finished signal
        cv.wait(lock, [this]() { return !queue.empty() || finished; });
        
        if (queue.empty()) return false;
        
        item = queue.front();
        queue.pop();
        cv.notify_all();
        return true;
    }

    // Signal that no more items will be added
    void set_finished() {
        std::unique_lock<std::mutex> lock(mutex);
        finished = true;
        cv.notify_all();
    }

    // Get current queue size
    size_t size() const {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.size();
    }

    // Check if queue is empty
    bool empty() const {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }

    // Clear the queue
    void clear() {
        std::unique_lock<std::mutex> lock(mutex);
        while (!queue.empty()) queue.pop();
        cv.notify_all();
    }
};

#endif // BOUNDED_QUEUE_H

