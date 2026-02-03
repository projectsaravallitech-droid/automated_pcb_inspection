#include <opencv2/opencv.hpp>
#include "VideoController.h"
#include "UIManager.h"

using namespace cv;
using namespace std;

int main()
{
    VideoController video;

    if (!video.openCamera(0)) {
        cout << "Camera error\n";
        return -1;
    }

    namedWindow("Video Stream", WINDOW_NORMAL);
    resizeWindow("Video Stream", 1000, 700);

    setMouseCallback("Video Stream", UIManager::mouseHandler, &video);

    Mat frame;
    Mat noVideo(480, 640, CV_8UC3, Scalar(50,50,50));

    while (true)
    {
        if (video.getStatus()) {
            frame = video.getFrame();
        } else {
            frame = noVideo.clone();
            putText(frame, "NO VIDEO",
                    Point(200,240),
                    FONT_HERSHEY_SIMPLEX,
                    1,
                    Scalar(0,0,255),
                    2);
        }

        UIManager::drawButtons(frame);

        imshow("Video Stream", frame);

        if (waitKey(30) == 27) // ESC to quit
            break;
    }

    destroyAllWindows();
    return 0;
}


