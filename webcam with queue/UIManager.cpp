#include "UIManager.h"

void UIManager::drawButtons(cv::Mat& frame)
{
    int r = 22;
    int spacing = 10;
    int y = frame.rows - 40;

    std::vector<std::string> labels =
        {"S","X","G","E","B","M","R","C","+","-"};

    std::vector<cv::Scalar> colors = {
        cv::Scalar(0,255,0),
        cv::Scalar(0,0,255),
        cv::Scalar(255,255,0),
        cv::Scalar(255,0,0),
        cv::Scalar(200,100,0),
        cv::Scalar(150,0,150),
        cv::Scalar(0,150,150),
        cv::Scalar(100,100,100),
        cv::Scalar(0,200,0),
        cv::Scalar(200,0,0)
    };

    int totalWidth = labels.size()*(2*r) + (labels.size()-1)*spacing;
    int startX = (frame.cols - totalWidth)/2;

    for (int i = 0; i < labels.size(); i++)
    {
        int cx = startX + i*(2*r + spacing);

        cv::circle(frame, cv::Point(cx, y), r, colors[i], cv::FILLED);

        cv::putText(frame,
                    labels[i],
                    cv::Point(cx-8, y+8),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.7,
                    cv::Scalar(0,0,0),
                    2);
    }
}

void UIManager::mouseHandler(int event, int x, int y, int flags, void* userdata)
{
    if (event != cv::EVENT_LBUTTONDOWN)
        return;

    VideoController* controller =
        static_cast<VideoController*>(userdata);

    int r = 22;
    int spacing = 10;

    int totalWidth = 10*(2*r) + 9*spacing;
    int startX = (640 - totalWidth)/2;   // assume 640 width if fixed
    int cy = 480 - 40;                  // assume 480 height if fixed

    auto inside = [&](int cx)
    {
        return (x-cx)*(x-cx) + (y-cy)*(y-cy) <= r*r;
    };

    for (int i = 0; i < 10; i++)
    {
        int cx = startX + i*(2*r + spacing);

        if (inside(cx))
        {
            switch(i)
            {
                case 0: break; // Start handled by Camera class
                case 1: break; // Stop handled by Camera class
                case 2: controller->toggleGray(); break;
                case 3: controller->toggleEdge(); break;
                case 4: controller->toggleBlur(); break;
                case 5: controller->toggleMorph(); break;
                case 6: controller->toggleRecording(); break;
                case 7: break; // capture handled in display
                case 8: controller->increaseBrightness(); break;
                case 9: controller->decreaseBrightness(); break;
            }
        }
    }
}
