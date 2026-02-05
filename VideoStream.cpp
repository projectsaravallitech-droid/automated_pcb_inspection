#include "VideoStream.h"
#include <iostream>

VideoStream::VideoStream() : stopRequested(false), running(false), connected(false) {}

VideoStream::~VideoStream() {
    stop();
}

void VideoStream::start(int cameraIndex) {
    if (running) return;

    stopRequested = false;
    running = true;
    connected = false;

    streamThread = std::thread(&VideoStream::streamLoop, this, cameraIndex);
}

void VideoStream::stop() {
    if (!running) return;

    stopRequested = true;
    if (streamThread.joinable()) {
        streamThread.join();
    }
    
    running = false;
    connected = false;
}

bool VideoStream::isRunning() const {
    return running;
}

bool VideoStream::isConnected() const {
    return connected;
}

bool VideoStream::readFrame(cv::Mat &frame) {
    std::lock_guard<std::mutex> lock(frameMutex);
    if (currentFrame.empty()) return false;
    
    currentFrame.copyTo(frame);
    return true;
}

void VideoStream::streamLoop(int cameraIndex) {
    cv::VideoCapture tempCap;
    
    if (!tempCap.open(cameraIndex)) {
        std::cerr << "Error: Could not open camera " << cameraIndex << std::endl;
        running = false;
        return;
    }

    cap = tempCap;
    connected = true;

    cv::Mat frame;
    while (!stopRequested) {
        if (!tempCap.read(frame)) {
            std::cerr << "Error: Camera disconnected or frame read failed." << std::endl;
            break;
        }

        if (frame.empty()) continue;

        {
            std::lock_guard<std::mutex> lock(frameMutex);
            frame.copyTo(currentFrame);
        }
    }

    tempCap.release();
    connected = false;
    running = false;
}