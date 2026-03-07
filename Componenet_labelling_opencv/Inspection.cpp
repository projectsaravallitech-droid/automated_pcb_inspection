#include "Inspection.h"

using namespace cv;

Inspection::Inspection()
{
    minArea = 200;   // filter small noise
}

Mat Inspection::detect(const Mat& frame)
{
    Mat gray, binary, morph;

    // Convert to grayscale
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // Convert to binary using OTSU
    threshold(gray, binary, 0, 255,
              THRESH_BINARY_INV | THRESH_OTSU);

    // Morphological closing
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3));
    morphologyEx(binary, morph, MORPH_CLOSE, kernel);

    // Connected Component Labelling
    Mat labels, stats, centroids;
    int numComponents = connectedComponentsWithStats(
                            morph,
                            labels,
                            stats,
                            centroids,
                            8,
                            CV_32S);

    Mat output = frame.clone();

    for (int i = 1; i < numComponents; i++)
    {
        int area = stats.at<int>(i, CC_STAT_AREA);
        if (area < minArea)
            continue;

        int x = stats.at<int>(i, CC_STAT_LEFT);
        int y = stats.at<int>(i, CC_STAT_TOP);
        int w = stats.at<int>(i, CC_STAT_WIDTH);
        int h = stats.at<int>(i, CC_STAT_HEIGHT);

        rectangle(output,
                  Rect(x, y, w, h),
                  Scalar(0, 255, 0),
                  2);

        putText(output,
                " ",
                Point(x, y - 5),
                FONT_HERSHEY_SIMPLEX,
                0.5,
                Scalar(0,255,0),
                1);
    }

    // BINARY SIDE
    

    Mat binaryColor;

    // Convert binary (1 channel) to 3 channel
    cvtColor(morph, binaryColor, COLOR_GRAY2BGR);

    // Resize binary to match original size (optional safety)
    resize(binaryColor, binaryColor, output.size());

    Mat combined;

    // Combine horizontally
    hconcat(output, binaryColor, combined);

    return combined;
}