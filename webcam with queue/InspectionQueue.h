#ifndef INSPECTION_QUEUE_H
#define INSPECTION_QUEUE_H

#include <opencv2/opencv.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>

class InspectionQueue
{
private:
    std::queue<cv::Mat> queue;
    std::mutex mtx;
    std::condition_variable cv;
    size_t maxSize;

public:
    InspectionQueue(size_t size = 5) : maxSize(size) {}

    void push(const cv::Mat& frame)
    {
        std::unique_lock<std::mutex> lock(mtx);

        if (queue.size() >= maxSize)
            queue.pop();

        queue.push(frame.clone());
        cv.notify_one();
    }

    cv::Mat pop()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return !queue.empty(); });

        cv::Mat f = queue.front();
        queue.pop();
        return f;
    }
};

#endif