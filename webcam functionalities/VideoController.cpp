#include "VideoController.h"

VideoController::VideoController() {
    isRunning = false;
    isGray = false;
    isEdge = false;
    isBlur = false;
    isMorph = false;
    isRecording = false;
    brightness = 0;
}

bool VideoController::openCamera(int index) {
    return cap.open(index);
}

void VideoController::start() { isRunning = true; }
void VideoController::stop() { isRunning = false; }
bool VideoController::getStatus() { return isRunning; }

void VideoController::toggleGray() { isGray = !isGray; }
void VideoController::toggleEdge() { isEdge = !isEdge; }
void VideoController::toggleBlur() { isBlur = !isBlur; }
void VideoController::toggleMorph() { isMorph = !isMorph; }

void VideoController::toggleRecording() {
    if (!isRecording) {
        writer.open("recorded.avi",
                    VideoWriter::fourcc('M','J','P','G'),
                    30,
                    Size(640,480));
        isRecording = true;
    } else {
        writer.release();
        isRecording = false;
    }
}

void VideoController::increaseBrightness() {
    brightness += 10;
}

void VideoController::decreaseBrightness() {
    brightness -= 10;
}

Mat VideoController::getFrame() {

    Mat frame;
    if (!isRunning) return frame;

    cap.read(frame);
    if (frame.empty()) return frame;

    // Brightness
    frame.convertTo(frame, -1, 1, brightness);

    Mat processed = frame.clone();  

    // Blur
    if (isBlur)
        GaussianBlur(processed, processed, Size(9,9), 0);

    // Edge Detection
    if (isEdge) {
        Mat gray, edges;
        cvtColor(processed, gray, COLOR_BGR2GRAY);
        Canny(gray, edges, 100, 200);
        cvtColor(edges, processed, COLOR_GRAY2BGR);
    }

    // Morphology
    if (isMorph) {
        Mat gray;
        cvtColor(processed, gray, COLOR_BGR2GRAY);
        Mat kernel = getStructuringElement(MORPH_RECT, Size(5,5));
        morphologyEx(gray, gray, MORPH_CLOSE, kernel);
        cvtColor(gray, processed, COLOR_GRAY2BGR);
    }

    // Grayscale 
    if (isGray) {
        Mat gray;
        cvtColor(processed, gray, COLOR_BGR2GRAY);
        cvtColor(gray, processed, COLOR_GRAY2BGR);
    }

    if (isRecording)
        writer.write(processed);

    return processed;
}
