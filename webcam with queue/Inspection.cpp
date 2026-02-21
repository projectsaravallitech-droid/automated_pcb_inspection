#include "Inspection.h"
#include <chrono>

Inspection::Inspection(InspectionQueue* in,
                       FrameQueue* out,
                       VideoController* vc)
{
    inputQueue = in;
    outputQueue = out;
    controller = vc;
    running = false;
}

void Inspection::start()
{
    running = true;
    inspectionThread = std::thread(&Inspection::inspectionLoop, this);
}

void Inspection::stop()
{
    running = false;

    if (inspectionThread.joinable())
        inspectionThread.join();
}

void Inspection::inspectionLoop()
{
    while (running)
    {
        cv::Mat raw = inputQueue->pop();

        if (!raw.empty())
        {
            // Low-level processing
            cv::Mat processed = controller->processFrame(raw);

            // Example inspection stage (placeholder)
            cv::Mat gray;
            cv::cvtColor(processed, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(gray, gray, 80, 160);
            cv::cvtColor(gray, processed, cv::COLOR_GRAY2BGR);

            outputQueue->push(processed);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}