#include "Inspection.h"
#include <vector>
#include <map>
#include <algorithm>

using namespace cv;
using namespace std;

Inspection::Inspection()
{
    minArea = 300;
}

Mat Inspection::detect(const Mat& frame)
{
    int rows = frame.rows;
    int cols = frame.cols;

    //  Manual Grayscale
    Mat gray(rows, cols, CV_8UC1);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Vec3b color = frame.at<Vec3b>(i, j);

            int grayVal = 0.299 * color[2] +
                          0.587 * color[1] +
                          0.114 * color[0];

            gray.at<uchar>(i, j) = (uchar)grayVal;
        }
    }

    //  Manual Threshold
    Mat binary(rows, cols, CV_8UC1);

    int thresh = 120;   // fixed threshold

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (gray.at<uchar>(i, j) < thresh)
                binary.at<uchar>(i, j) = 255;
            else
                binary.at<uchar>(i, j) = 0;
        }
    }

    // Manual CCL (Two-Pass)

    Mat labels = Mat::zeros(rows, cols, CV_32S);
    int label = 1;

    map<int, int> parent;  // union-find

    auto findRoot = [&](int x)
    {
        while (parent[x] != x)
            x = parent[x];
        return x;
    };

    auto unite = [&](int a, int b)
    {
        int rootA = findRoot(a);
        int rootB = findRoot(b);
        if (rootA != rootB)
            parent[rootB] = rootA;
    };

    // PASS 1
    for (int i = 1; i < rows; i++)
    {
        for (int j = 1; j < cols; j++)
        {
            if (binary.at<uchar>(i, j) == 255)
            {
                vector<int> neighbors;

                if (binary.at<uchar>(i, j - 1) == 255)
                    neighbors.push_back(labels.at<int>(i, j - 1));

                if (binary.at<uchar>(i - 1, j) == 255)
                    neighbors.push_back(labels.at<int>(i - 1, j));

                if (neighbors.empty())
                {
                    labels.at<int>(i, j) = label;
                    parent[label] = label;
                    label++;
                }
                else
                {
                    int minLabel = *min_element(neighbors.begin(), neighbors.end());
                    labels.at<int>(i, j) = minLabel;

                    for (int n : neighbors)
                        unite(minLabel, n);
                }
            }
        }
    }

    // PASS 2
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (labels.at<int>(i, j) > 0)
                labels.at<int>(i, j) = findRoot(labels.at<int>(i, j));
        }
    }

    //  Bounding Box Calculation

    map<int, int> area;
    map<int, int> minX, minY, maxX, maxY;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int lbl = labels.at<int>(i, j);
            if (lbl > 0)
            {
                area[lbl]++;

                if (minX.find(lbl) == minX.end())
                {
                    minX[lbl] = maxX[lbl] = j;
                    minY[lbl] = maxY[lbl] = i;
                }
                else
                {
                    minX[lbl] = min(minX[lbl], j);
                    maxX[lbl] = max(maxX[lbl], j);
                    minY[lbl] = min(minY[lbl], i);
                    maxY[lbl] = max(maxY[lbl], i);
                }
            }
        }
    }

    Mat output = frame.clone();

    for (auto& comp : area)
    {
        int lbl = comp.first;
        int compArea = comp.second;

        if (compArea < minArea)
            continue;

        rectangle(output,
                  Point(minX[lbl], minY[lbl]),
                  Point(maxX[lbl], maxY[lbl]),
                  Scalar(0,255,0),
                  2);
    }

    return output;
}