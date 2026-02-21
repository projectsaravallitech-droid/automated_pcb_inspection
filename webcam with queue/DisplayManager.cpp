#include "DisplayManager.h"

DisplayManager::DisplayManager(FrameQueue* fq,
                               VideoController* vc)
{
    queue = fq;
    controller = vc;  
    running = false;
}

void DisplayManager::start()
{
    running = true;
    displayThread = std::thread(&DisplayManager::displayLoop, this);
}

void DisplayManager::stop()
{
    running = false;

    if (displayThread.joinable())
        displayThread.join();
}

void DisplayManager::displayLoop()
{
    cv::namedWindow("Video Stream", cv::WINDOW_NORMAL);
    cv::resizeWindow("Video Stream", 1000, 700);

    cv::setMouseCallback("Video Stream",
                         UIManager::mouseHandler,
                         controller);

    while (running)
    {
        cv::Mat frame = queue->pop();

        UIManager::drawButtons(frame);

        cv::imshow("Video Stream", frame);

        if (cv::waitKey(33) == 27)
            running = false;
    }

    cv::destroyAllWindows();
}
