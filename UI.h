#ifndef UI_H
#define UI_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include "VideoStream.h"
#include "BoundedQueue.h"
#include "Inspection.h"

enum Action {
    ACTION_NONE,
    ACTION_START,
    ACTION_STOP,
    ACTION_CLOSE,
    ACTION_GRAYSCALE,
    ACTION_CAPTURE,
    ACTION_RECORD,
    ACTION_INSPECTION
};

struct Button {
    cv::Rect rect;
    std::string text;
    Action action;
    cv::Scalar color;
};

class UI {
public:
    UI(VideoStream& stream, const std::string& noSignalPath = "images.png");
    ~UI();

    void run();
    
    void wait();

private:
    VideoStream& stream;
    std::thread displayThread;
    std::thread inspectionThread;
    std::atomic<bool> shouldExit;
    
    cv::Mat noSignalImage;
    std::vector<Button> buttons;
    const std::string windowName = "USB Video Stream";
    BoundedQueue<cv::Mat> frameQueue;
    BoundedQueue<std::pair<cv::Mat, std::vector<Detection>>> processedQueue;

    // New members
    Inspection inspector;
    bool isGrayscale;
    bool isRecording;
    bool isInspection;
    bool captureRequested = false; // Initialize inline or in constructor
    cv::VideoWriter videoWriter;

    void displayLoop();
    void inspectionLoop();
    void setupButtons();
    void drawButtons(cv::Mat &img, bool isStreamActive);
    Action checkClick(int x, int y, bool isStreamActive);
    
    void saveImage(const cv::Mat& frame);
    void toggleRecording(const cv::Mat& frame);
    
    static void onMouse(int event, int x, int y, int flags, void* userdata);
};

#endif