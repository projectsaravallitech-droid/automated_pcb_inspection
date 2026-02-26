#ifndef INSPECTION_H
#define INSPECTION_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>

struct Detection
{
    int class_id;
    float confidence;
    cv::Rect box;
};

class Inspection
{
public:
    Inspection(const std::string& modelPath);
    void detect(cv::Mat& frame);

private:
    cv::dnn::Net net;
    std::vector<std::string> classNames;

    const float SCORE_THRESHOLD = 0.85f;
    const float NMS_THRESHOLD = 0.4f;
    const cv::Size MODEL_SIZE = cv::Size(640, 640);

    std::vector<std::string> load_class_list();
};

#endif
