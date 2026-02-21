#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include "InspectionQueue.h"   // ✅ correct queue

class Camera
{
private:
    std::thread cameraThread;
    std::atomic<bool> running;

    cv::VideoCapture cap;

    InspectionQueue* queue;   // raw frame queue

    void captureLoop();

public:
    Camera(InspectionQueue* iq);   // ✅ controller removed

    void start();
    void stop();
};

#endif