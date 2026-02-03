#include "UIManager.h"
#include <vector>
#include <string>

using namespace std;

void UIManager::drawButtons(Mat &frame)
{
    int r = 22;              // slightly smaller
    int spacing = 10;        // gap between buttons
    int startX = 30;         // left margin
    int y = frame.rows - 50; // bottom position

    vector<string> labels = {"S","X","G","E","B","M","R","C","+","-"};
    vector<Scalar> colors = {
        Scalar(0,255,0),    // S
        Scalar(0,0,255),    // X
        Scalar(255,255,0),  // G
        Scalar(255,0,0),    // E
        Scalar(200,100,0),  // B
        Scalar(150,0,150),  // M
        Scalar(0,150,150),  // R
        Scalar(100,100,100),// C
        Scalar(0,200,0),    // +
        Scalar(200,0,0)     // -
    };

    for (int i = 0; i < labels.size(); i++)
    {
        int cx = startX + i * (2*r + spacing);

        circle(frame, Point(cx, y), r, colors[i], FILLED);

        putText(frame,
                labels[i],
                Point(cx - 8, y + 8),
                FONT_HERSHEY_SIMPLEX,
                0.7,
                Scalar(0,0,0),
                2);
    }
}

void UIManager::mouseHandler(int event, int x, int y, int flags, void* userdata)
{
    if (event != EVENT_LBUTTONDOWN)
        return;

    VideoController* video = (VideoController*)userdata;

    int r = 22;
    int spacing = 10;
    int startX = 30;
    int cy = 480 - 50;   // if fixed size

    auto inside = [&](int cx, int cy) {
        return (x-cx)*(x-cx) + (y-cy)*(y-cy) <= r*r;
    };

    for (int i = 0; i < 10; i++)
    {
        int cx = startX + i * (2*r + spacing);

        if (inside(cx, cy))
        {
            switch(i)
            {
                case 0: video->start(); break;
                case 1: video->stop(); break;
                case 2: video->toggleGray(); break;
                case 3: video->toggleEdge(); break;
                case 4: video->toggleBlur(); break;
                case 5: video->toggleMorph(); break;
                case 6: video->toggleRecording(); break;
                case 7: {
                    Mat frame = video->getFrame();
                    if (!frame.empty())
                        imwrite("capture.jpg", frame);
                    break;
                }
                case 8: video->increaseBrightness(); break;
                case 9: video->decreaseBrightness(); break;
            }
        }
    }
}