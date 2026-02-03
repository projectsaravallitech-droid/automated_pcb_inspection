#ifndef VIDEO_CONTROLLER_H
#define VIDEO_CONTROLLER_H

#include <opencv2/opencv.hpp>
using namespace cv;

class VideoController {
private:
    VideoCapture cap;
    VideoWriter writer;

    bool isRunning;
    bool isGray;
    bool isEdge;
    bool isBlur;
    bool isMorph;
    bool isRecording;

    int brightness;

public:
    VideoController();

    bool openCamera(int index);

    void start();
    void stop();
    bool getStatus();

    void toggleGray();
    void toggleEdge();
    void toggleBlur();
    void toggleMorph();
    void toggleRecording();

    void increaseBrightness();
    void decreaseBrightness();

    Mat getFrame();
};

#endif
