#include "Camera.h"
#include <chrono>

Camera::Camera(InspectionQueue* iq)
{
    queue = iq;
    running = false;
}

void Camera::start()
{
    if (!cap.open(0))
        return;

    running = true;
    cameraThread = std::thread(&Camera::captureLoop, this);
}

void Camera::stop()
{
    running = false;

    if (cameraThread.joinable())
        cameraThread.join();

    cap.release();
}

void Camera::captureLoop()
{
    while (running)
    {
        cv::Mat rawFrame;
        cap.read(rawFrame);

        if (!rawFrame.empty())
        {
            queue->push(rawFrame);   // RAW frame → InspectionQueue
        }

        // ~10 FPS
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
    }
}