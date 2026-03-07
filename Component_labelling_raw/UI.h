#pragma once
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include "VideoStream.h"
#include "Inspection.h"
#include "BoundedQueue.h"

class UI
{
public:
    UI();
    void run();

private:
    void inspectionLoop();
    void displayLoop();

    VideoStream stream;
    Inspection inspector;

    BoundedQueue<cv::Mat> frameQueue;
    BoundedQueue<cv::Mat> processedQueue;

    std::thread inspectionThread;
    std::thread displayThread;

    std::atomic<bool> running;
};