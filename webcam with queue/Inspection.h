#ifndef INSPECTION_H
#define INSPECTION_H

#include <thread>
#include <atomic>
#include "InspectionQueue.h"
#include "FrameQueue.h"
#include "VideoController.h"

class Inspection
{
private:
    std::thread inspectionThread;
    std::atomic<bool> running;

    InspectionQueue* inputQueue;
    FrameQueue* outputQueue;
    VideoController* controller;

    void inspectionLoop();

public:
    Inspection(InspectionQueue* in,
               FrameQueue* out,
               VideoController* vc);

    void start();
    void stop();
};

#endif