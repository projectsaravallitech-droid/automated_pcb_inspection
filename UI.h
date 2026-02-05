#ifndef UI_H
#define UI_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include "VideoStream.h"

enum Action {
    ACTION_NONE,
    ACTION_START,
    ACTION_STOP,
    ACTION_CLOSE
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
    std::atomic<bool> shouldExit;
    
    cv::Mat noSignalImage;
    std::vector<Button> buttons;
    const std::string windowName = "USB Video Stream";

    void displayLoop();
    void setupButtons();
    void drawButtons(cv::Mat &img, bool isStreamActive);
    Action checkClick(int x, int y, bool isStreamActive);
    
    static void onMouse(int event, int x, int y, int flags, void* userdata);
};

#endif