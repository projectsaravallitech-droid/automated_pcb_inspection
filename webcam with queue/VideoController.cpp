#include "VideoController.h"

VideoController::VideoController()
{
    isGray = false;
    isEdge = false;
    isBlur = false;
    isMorph = false;
    isRecording = false;
    brightness = 0;
}

void VideoController::toggleGray() { isGray = !isGray; }
void VideoController::toggleEdge() { isEdge = !isEdge; }
void VideoController::toggleBlur() { isBlur = !isBlur; }
void VideoController::toggleMorph() { isMorph = !isMorph; }

void VideoController::increaseBrightness() { brightness += 10; }
void VideoController::decreaseBrightness() { brightness -= 10; }

void VideoController::toggleRecording()
{
    if (!isRecording)
    {
        writer.open("recorded.avi",
                    cv::VideoWriter::fourcc('M','J','P','G'),
                    10,
                    cv::Size(640,480));
        isRecording = true;
    }
    else
    {
        writer.release();
        isRecording = false;
    }
}

cv::Mat VideoController::processFrame(const cv::Mat& input)
{
    if (input.empty())
        return input;

    cv::Mat frame = input.clone();

    // Brightness
    frame.convertTo(frame, -1, 1, brightness);

    // Blur
    if (isBlur)
        cv::GaussianBlur(frame, frame, cv::Size(9,9), 0);

    // Edge
    if (isEdge)
    {
        cv::Mat gray, edges;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::Canny(gray, edges, 100, 200);
        cv::cvtColor(edges, frame, cv::COLOR_GRAY2BGR);
    }

    // Morphology
    if (isMorph)
    {
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5));
        cv::morphologyEx(gray, gray, cv::MORPH_CLOSE, kernel);
        cv::cvtColor(gray, frame, cv::COLOR_GRAY2BGR);
    }

    // Grayscale (apply last)
    if (isGray)
    {
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(gray, frame, cv::COLOR_GRAY2BGR);
    }

    if (isRecording)
        writer.write(frame);

    return frame;
}

void VideoController::captureFrame(const cv::Mat& frame)
{
    if (!frame.empty())
        cv::imwrite("capture.jpg", frame);
}
