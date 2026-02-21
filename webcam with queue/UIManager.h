#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <opencv2/opencv.hpp>
#include "VideoController.h"

class UIManager
{
public:
    static void drawButtons(cv::Mat& frame);
    static void mouseHandler(int event, int x, int y, int flags, void* userdata);
};

#endif
