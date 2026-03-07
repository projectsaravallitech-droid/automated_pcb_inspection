#pragma once
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include "BoundedQueue.h"

class VideoStream
{
public:
    VideoStream();
    ~VideoStream();

    void start();
    void stop();
    void setQueue(BoundedQueue<cv::Mat>* q);

private:
    void captureLoop();

    cv::VideoCapture cap;
    std::thread captureThread;
    std::atomic<bool> running;

    BoundedQueue<cv::Mat>* frameQueue;
};