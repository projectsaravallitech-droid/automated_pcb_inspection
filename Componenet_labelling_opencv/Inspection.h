#pragma once
#include <opencv2/opencv.hpp>

class Inspection
{
public:
    Inspection();
    cv::Mat detect(const cv::Mat& frame);

private:
    int minArea;
};