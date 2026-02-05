#ifndef VIDEO_STREAM_H
#define VIDEO_STREAM_H

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <atomic>

class VideoStream {
private:
    cv::VideoCapture cap;
    std::thread streamThread;
    std::mutex frameMutex;
    cv::Mat currentFrame;
    
    std::atomic<bool> stopRequested;
    std::atomic<bool> running;
    std::atomic<bool> connected;

    void streamLoop(int cameraIndex);

public:
    VideoStream();
    ~VideoStream();

    void start(int cameraIndex = 0);
    
    void stop();
    
    bool isRunning() const;
    
    bool isConnected() const;

    bool readFrame(cv::Mat &frame);
};

#endif