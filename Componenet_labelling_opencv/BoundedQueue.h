#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class BoundedQueue
{
public:
    BoundedQueue(size_t maxSize = 5) : maxSize(maxSize) {}

    void enqueue(const T& item)
    {
        std::unique_lock<std::mutex> lock(mtx);
        condFull.wait(lock, [this]() { return queue.size() < maxSize; });
        queue.push(item);
        condEmpty.notify_one();
    }

    T dequeue()
    {
        std::unique_lock<std::mutex> lock(mtx);
        condEmpty.wait(lock, [this]() { return !queue.empty(); });
        T item = queue.front();
        queue.pop();
        condFull.notify_one();
        return item;
    }

private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable condEmpty, condFull;
    size_t maxSize;
};