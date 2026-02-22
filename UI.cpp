#include "UI.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace cv;
using namespace std;

UI::UI(VideoStream& streamRef, const string& imagePath) 
    : stream(streamRef), shouldExit(false), 
      frameQueue(5), processedQueue(3),
      inspector("yolo11n.onnx"), 
      isGrayscale(false), isRecording(false), isInspection(false), captureRequested(false) {
    
    noSignalImage = imread(imagePath);
    if (noSignalImage.empty()) {
        noSignalImage = Mat::zeros(480, 640, CV_8UC3);
        putText(noSignalImage, "NO SIGNAL", Point(200, 240), 
                FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    } else {
        if (noSignalImage.cols != 640 || noSignalImage.rows != 480)
            resize(noSignalImage, noSignalImage, Size(640, 480));
    }


    stream.setFrameQueue(&frameQueue);
    setupButtons();
}

UI::~UI() {
    shouldExit = true;
    if (displayThread.joinable()) {
        displayThread.join();
    }
    if (inspectionThread.joinable()) {
        inspectionThread.join();
    }
}

void UI::run() {
    shouldExit = false;
    inspectionThread = std::thread(&UI::inspectionLoop, this);
    displayThread = std::thread(&UI::displayLoop, this);
}

void UI::wait() {
    if (inspectionThread.joinable()) {
        inspectionThread.join();
    }
    if (displayThread.joinable()) {
        displayThread.join();
    }
}

void UI::setupButtons() {
    buttons.clear();
    // Row 1 (below video at y=480)
    buttons.push_back({Rect(50, 490, 100, 40), "START", ACTION_START, Scalar(0, 200, 0)});
    buttons.push_back({Rect(200, 490, 100, 40), "STOP", ACTION_STOP, Scalar(0, 0, 200)});
    buttons.push_back({Rect(490, 490, 100, 40), "CLOSE", ACTION_CLOSE, Scalar(100, 100, 100)});
    
    // Row 2 (below row 1)
    buttons.push_back({Rect(50, 535, 100, 40), "GRAY", ACTION_GRAYSCALE, Scalar(150, 150, 150)});
    buttons.push_back({Rect(200, 535, 100, 40), "CAP", ACTION_CAPTURE, Scalar(200, 200, 0)});
    buttons.push_back({Rect(350, 535, 100, 40), "REC", ACTION_RECORD, Scalar(0, 200, 200)});
    buttons.push_back({Rect(490, 535, 100, 40), "INSPECT", ACTION_INSPECTION, Scalar(200, 0, 200)});
}

void UI::inspectionLoop() {
    // This thread processes frames at 30 fps (detection on all frames)
    Mat frame;
    
    while (!shouldExit) {
        // Get frame from capture queue (30 fps input)
        if (frameQueue.tryDequeue(frame)) {
            // Run detection on the frame (30 fps)
            vector<Detection> detections;
            if (isInspection) {
                inspector.detect(frame);
                // The inspector modifies the frame with bounding boxes
            }
            
            // Add to processed queue for display
            processedQueue.enqueue(make_pair(frame, detections));
        } else {
            // No frame available, sleep briefly
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }
    
    processedQueue.set_finished();
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
        case ACTION_GRAYSCALE:
            ui->isGrayscale = !ui->isGrayscale;
            break;
        case ACTION_CAPTURE:
            ui->captureRequested = true;
            break; 
        case ACTION_RECORD:
            ui->isRecording = !ui->isRecording;
            break;
        case ACTION_INSPECTION:
            ui->isInspection = !ui->isInspection;
            break;
        default:
            break;
    }
}

void UI::displayLoop() {
    namedWindow(windowName);
    setMouseCallback(windowName, onMouse, this);

    Mat frame;
    Mat displayFrame = noSignalImage.clone(); 
    
    // Create a canvas larger than the video to hold buttons below it
    Mat canvas(600, 640, CV_8UC3, Scalar(50, 50, 50));
    
    int frameCount = 0; // Frame counter for throttling to 10 fps

    while (!shouldExit) {
        auto startTime = std::chrono::steady_clock::now();

        bool isRunning = stream.isRunning();
        bool isConnected = stream.isConnected();

        // Try to get a new frame from processed queue (runs at 30 fps internally)
        // But we only display every 3rd frame (10 fps)
        if (isRunning && isConnected) {
            std::pair<Mat, vector<Detection>> processed;
            
            if (processedQueue.tryDequeue(processed)) {
                Mat frame = processed.first;
                vector<Detection> detections = processed.second;
                
                displayFrame = frame.clone();
                
                if (displayFrame.cols != 640 || displayFrame.rows != 480)
                    resize(displayFrame, displayFrame, Size(640, 480));
                    
                // Apply features
                if (isGrayscale) {
                    cvtColor(displayFrame, displayFrame, COLOR_BGR2GRAY);
                    cvtColor(displayFrame, displayFrame, COLOR_GRAY2BGR);
                }
                
                if (captureRequested) {
                    saveImage(displayFrame);
                    captureRequested = false;
                    rectangle(displayFrame, Rect(0,0,640,480), Scalar(255,255,255), 10);
                }
                
                if (isRecording) {
                    if (!videoWriter.isOpened()) {
                        // Start recording
                        std::string filename = "rec_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".avi";
                        videoWriter.open(filename, VideoWriter::fourcc('M','J','P','G'), 10, Size(640, 480));
                    }
                    if (videoWriter.isOpened()) {
                        videoWriter.write(displayFrame);
                        // Add recording indicator
                        circle(displayFrame, Point(620, 20), 10, Scalar(0, 0, 255), -1);
                    }
                } else {
                    if (videoWriter.isOpened()) {
                        videoWriter.release();
                    }
                }
                frameCount++;
            }
        } else {
            displayFrame = noSignalImage.clone();
            if (displayFrame.cols != 640 || displayFrame.rows != 480)
                resize(displayFrame, displayFrame, Size(640, 480));
                
            if (isRunning && !isConnected) {
                putText(displayFrame, "CONNECTING...", Point(220, 300), 
                       FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 255), 2);
            }
        }
        
        // Clear canvas
        canvas = Scalar(50, 50, 50);
        
        // Copy displayFrame to canvas top
        if (!displayFrame.empty()) {
            displayFrame.copyTo(canvas(Rect(0, 0, 640, 480)));
        }
        
        drawButtons(canvas, isRunning);
        imshow(windowName, canvas);

        // Handle UI events
        char key = (char)waitKey(1);

        if (key == 'q') {
            shouldExit = true;
        } else if (key == 's') {
            if (isRunning) stream.stop();
        } else if (key == 'r') {
            if (!isRunning) stream.start(0);
        }

        auto endTime = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
        
        int sleepTime = 100 - (int)elapsed.count(); // 100ms for 10 FPS
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }
    }
    
    if (videoWriter.isOpened()) videoWriter.release();
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

void UI::saveImage(const cv::Mat& frame) {
    if (frame.empty()) return;
    std::string filename = "cap_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".png";
    imwrite(filename, frame);
    std::cout << "Saved " << filename << std::endl;
}

void UI::toggleRecording(const cv::Mat& frame) {
    isRecording = !isRecording;
}

Action UI::checkClick(int x, int y, bool isStreamActive) {
    for (const auto &btn : buttons) {
        if (btn.rect.contains(Point(x, y))) {
            return btn.action;
        }
    }
    return ACTION_NONE;
}
