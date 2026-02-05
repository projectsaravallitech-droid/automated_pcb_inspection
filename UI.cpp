#include "UI.h"
#include <iostream>

using namespace cv;
using namespace std;

UI::UI(VideoStream& streamRef, const string& imagePath) 
    : stream(streamRef), shouldExit(false) {
    
    noSignalImage = imread(imagePath);
    if (noSignalImage.empty()) {
        noSignalImage = Mat::zeros(480, 640, CV_8UC3);
        putText(noSignalImage, "NO SIGNAL", Point(200, 240), 
                FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    } else {
        if (noSignalImage.cols != 640 || noSignalImage.rows != 480)
            resize(noSignalImage, noSignalImage, Size(640, 480));
    }

    setupButtons();
}

UI::~UI() {
    shouldExit = true;
    if (displayThread.joinable()) {
        displayThread.join();
    }
}

void UI::run() {
    shouldExit = false;
    displayThread = std::thread(&UI::displayLoop, this);
}

void UI::wait() {
    if (displayThread.joinable()) {
        displayThread.join();
    }
}

void UI::setupButtons() {
    buttons.clear();
    buttons.push_back({Rect(50, 400, 100, 40), "START", ACTION_START, Scalar(0, 200, 0)});
    buttons.push_back({Rect(200, 400, 100, 40), "STOP", ACTION_STOP, Scalar(0, 0, 200)});
    buttons.push_back({Rect(490, 400, 100, 40), "CLOSE", ACTION_CLOSE, Scalar(100, 100, 100)});
}

void UI::onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event != EVENT_LBUTTONDOWN) return;

    UI* ui = (UI*)userdata;
    bool isRunning = ui->stream.isRunning();
    
    Action action = ui->checkClick(x, y, isRunning);
    
    switch (action) {
        case ACTION_START:
            if (!isRunning) ui->stream.start(0);
            break;
        case ACTION_STOP:
            if (isRunning) ui->stream.stop();
            break;
        case ACTION_CLOSE:
            ui->shouldExit = true;
            break;
        default:
            break;
    }
}

void UI::displayLoop() {
    namedWindow(windowName);
    setMouseCallback(windowName, onMouse, this);

    Mat frame;
    
    while (!shouldExit) {
        Mat displayFrame;
        bool isRunning = stream.isRunning();
        bool isConnected = stream.isConnected();

        if (isRunning) {
            if (isConnected) {
                if (stream.readFrame(frame)) {
                    displayFrame = frame.clone();
                    if (displayFrame.cols != 640 || displayFrame.rows != 480)
                        resize(displayFrame, displayFrame, Size(640, 480));
                }
            } else {
                displayFrame = noSignalImage.clone();
                putText(displayFrame, "CONNECTING...", Point(220, 300), 
                        FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 255), 2);
            }
        } 
        
        if (!isRunning || displayFrame.empty()) {
            displayFrame = noSignalImage.clone();
        }

        if (!displayFrame.empty()) {
            drawButtons(displayFrame, isRunning);
            imshow(windowName, displayFrame);
        }

        char key = (char)waitKey(30);

        if (key == 'q') {
            shouldExit = true;
        } else if (key == 's') {
            if (isRunning) stream.stop();
        } else if (key == 'r') {
            if (!isRunning) stream.start(0);
        }
    }
    
    destroyAllWindows();
}

void UI::drawButtons(Mat &img, bool isStreamActive) {
    for (const auto &btn : buttons) {
        Scalar color = btn.color;
        if (isStreamActive && btn.action == ACTION_START) color = Scalar(100, 150, 100);
        if (!isStreamActive && btn.action == ACTION_STOP) color = Scalar(100, 100, 150);

        rectangle(img, btn.rect, color, -1);
        rectangle(img, btn.rect, Scalar(255, 255, 255), 2);

        int baseline = 0;
        Size textSize = getTextSize(btn.text, FONT_HERSHEY_SIMPLEX, 0.6, 2, &baseline);
        Point textOrg(btn.rect.x + (btn.rect.width - textSize.width) / 2,
                      btn.rect.y + (btn.rect.height + textSize.height) / 2);
        
        putText(img, btn.text, textOrg, FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
    }
}

Action UI::checkClick(int x, int y, bool isStreamActive) {
    for (const auto &btn : buttons) {
        if (btn.rect.contains(Point(x, y))) {
            return btn.action;
        }
    }
    return ACTION_NONE;
}