#include "VideoStream.h"

using namespace cv;

VideoStream::VideoStream()
{
    running = false;
    frameQueue = nullptr;
}

VideoStream::~VideoStream()
{
    stop();
}

void VideoStream::setQueue(BoundedQueue<Mat>* q)
{
    frameQueue = q;
}

void VideoStream::start()
{
    if (running) return;

    cap.open(0);   // USB camera
    if (!cap.isOpened())
        return;

    running = true;
    captureThread = std::thread(&VideoStream::captureLoop, this);
}

void VideoStream::stop()
{
    running = false;
    if (captureThread.joinable())
        captureThread.join();

    if (cap.isOpened())
        cap.release();
}

void VideoStream::captureLoop()
{
    Mat frame;

    while (running)
    {
        cap >> frame;
        if (frame.empty()) continue;

        if (frameQueue)
            frameQueue->enqueue(frame.clone());

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}