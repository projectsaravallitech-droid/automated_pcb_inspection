#include "VideoStream.h"
#include <iostream>

VideoStream::VideoStream() : stopRequested(false), running(false), connected(false), frameQueue(nullptr) {}

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
        auto startTime = std::chrono::steady_clock::now();

        if (!tempCap.read(frame)) {
            std::cerr << "Error: Camera disconnected or frame read failed." << std::endl;
            break;
        }

        if (frame.empty()) continue;

        if (frameQueue) {
            // Push to queue for 30 fps capture
            frameQueue->enqueue(frame.clone()); 
        } else {
             // Fallback to old behavior if no queue is set (compatibility)
            std::lock_guard<std::mutex> lock(frameMutex);
            frame.copyTo(currentFrame);
        }

        // Enforce ~30 FPS (33ms per frame)
        auto endTime = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
        
        int sleepTime = 33 - (int)elapsed.count(); // 33ms for 30 FPS
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }
    }

    tempCap.release();
    connected = false;
    running = false;
}

void VideoStream::setFrameQueue(BoundedQueue<cv::Mat>* queue) {
    frameQueue = queue;
}
