#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include "FrameQueue.h"
#include "VideoController.h"
#include "UIManager.h"

class DisplayManager
{
private:
    std::thread displayThread;
    std::atomic<bool> running;

    FrameQueue* queue;
    VideoController* controller;  

    void displayLoop();

public:
    DisplayManager(FrameQueue* fq,
                   VideoController* vc); 

    void start();
    void stop();
};

#endif
