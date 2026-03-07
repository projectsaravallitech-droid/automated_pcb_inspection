#include "UI.h"

using namespace cv;

UI::UI() : frameQueue(5), processedQueue(5)
{
    running = true;
}

void UI::run()
{
    stream.setQueue(&frameQueue);
    stream.start();

    inspectionThread = std::thread(&UI::inspectionLoop, this);
    displayThread = std::thread(&UI::displayLoop, this);

    inspectionThread.join();
    displayThread.join();

    stream.stop();
}

void UI::inspectionLoop()
{
    while (running)
    {
        Mat frame = frameQueue.dequeue();
        Mat detected = inspector.detect(frame);
        processedQueue.enqueue(detected);
    }
}

void UI::displayLoop()
{
    while (running)
    {
        Mat frame = processedQueue.dequeue();
        imshow("PCB Component Labelling (CCL)", frame);

        if (waitKey(1) == 27)  // ESC key
            running = false;
    }

    destroyAllWindows();
}