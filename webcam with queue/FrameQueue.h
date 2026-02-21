#ifndef FRAME_QUEUE_H
#define FRAME_QUEUE_H

#include <opencv2/opencv.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>

class FrameQueue {
private:
    std::queue<cv::Mat> queue;
    std::mutex mtx;
    std::condition_variable cv;
    size_t maxSize;

public:
    FrameQueue(size_t size = 5) : maxSize(size) {}

    void push(const cv::Mat& frame) {
        std::unique_lock<std::mutex> lock(mtx);

        if (queue.size() >= maxSize)
            queue.pop();   // drop oldest frame

        queue.push(frame.clone());
        cv.notify_one();
    }

    cv::Mat pop() {
        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [this] { return !queue.empty(); });

        cv::Mat frame = queue.front();
        queue.pop();
        return frame;
    }
};

#endif
