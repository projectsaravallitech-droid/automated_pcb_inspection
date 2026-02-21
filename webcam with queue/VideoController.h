#ifndef VIDEO_CONTROLLER_H
#define VIDEO_CONTROLLER_H

#include <opencv2/opencv.hpp>

class VideoController
{
private:
    bool isGray;
    bool isEdge;
    bool isBlur;
    bool isMorph;
    bool isRecording;

    int brightness;

    cv::VideoWriter writer;

public:
    VideoController();

    // Toggle operations
    void toggleGray();
    void toggleEdge();
    void toggleBlur();
    void toggleMorph();
    void toggleRecording();

    void increaseBrightness();
    void decreaseBrightness();

    // Processing function
    cv::Mat processFrame(const cv::Mat& input);

    // Capture image
    void captureFrame(const cv::Mat& frame);
};

#endif
