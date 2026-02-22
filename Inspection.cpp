#include "Inspection.h"
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;
using namespace cv::dnn;

Inspection::Inspection(const std::string& modelPath)
{
    net = readNetFromONNX(modelPath);

    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);

    classNames = load_class_list();

    cout << "Model loaded. Classes: " << classNames.size() << endl;
}

vector<string> Inspection::load_class_list()
{
    vector<string> names;
    ifstream ifs("config_files/classes.txt");
    string line;

    while (getline(ifs, line))
        names.push_back(line);

    return names;
}

void Inspection::detect(Mat& frame)
{
    if (frame.empty()) return;
    Mat blob;

    blobFromImage(frame, blob, 1.0 / 255.0, MODEL_SIZE, Scalar(), true, false);

    net.setInput(blob);

    vector<Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    // --- UNIVERSAL TENSOR PARSING ---
    // OpenCV gives us a tensor, usually 1 x DIMENSIONS x ROWS  or  1 x ROWS x DIMENSIONS
    // We want to normalize it so we always process rows of individual anchor boxes.
    
    int dim1 = outputs[0].size[1];
    int dim2 = outputs[0].size[2];
    
    // Feature dimension is typically 80 or 84 (class scores + box coords)
    // Anchor/row dimension is typically much larger (8400 for 640x640 input)
    int featureDim, numAnchorBoxes;
    
    if (dim1 < dim2) {
        // 1 x 80/84 x 8400
        featureDim = dim1;
        numAnchorBoxes = dim2;
    } else {
        // 1 x 8400 x 80/84
        featureDim = dim2;
        numAnchorBoxes = dim1;
    }

    vector<Rect> detections;
    vector<float> confidences;
    vector<int> classIds;

    float* pdata = (float*)outputs[0].data;

    for (int i = 0; i < numAnchorBoxes; i++) {
        float* row_ptr;
        
        if (dim1 < dim2) {
            row_ptr = pdata + i;
        } else {
            row_ptr = pdata + i * featureDim;
        }

        // Box coordinates (cx, cy, w, h)
        float cx = row_ptr[0];
        float cy = row_ptr[1];
        float w = row_ptr[2];
        float h = row_ptr[3];

        // Class scores start at index 4
        int classId = 0;
        float maxScore = 0.0f;

        for (int c = 4; c < featureDim; c++) {
            float score;
            if (dim1 < dim2) {
                score = pdata[c * numAnchorBoxes + i];
            } else {
                score = row_ptr[c];
            }
            
            if (score > maxScore) {
                maxScore = score;
                classId = c - 4;
            }
        }

        if (maxScore > SCORE_THRESHOLD) {
            // Convert from center form to corner form
            float x = (cx - w * 0.5f) * frame.cols;
            float y = (cy - h * 0.5f) * frame.rows;
            float width = w * frame.cols;
            float height = h * frame.rows;

            detections.push_back(Rect((int)x, (int)y, (int)width, (int)height));
            confidences.push_back(maxScore);
            classIds.push_back(classId);
        }
    }

    vector<int> indices;
    NMSBoxes(detections, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);

    // Draw bounding boxes
    for (int idx : indices) {
        const auto& box = detections[idx];
        float confidence = confidences[idx];
        int classId = classIds[idx];

        // Ensure box is within frame bounds
        int x = max(0, box.x);
        int y = max(0, box.y);
        int x2 = min(frame.cols - 1, box.x + box.width);
        int y2 = min(frame.rows - 1, box.y + box.height);

        if (x >= x2 || y >= y2) continue;

        rectangle(frame, Point(x, y), Point(x2, y2), Scalar(0, 255, 0), 2);

        string label;
        if (classId < (int)classNames.size())
            label = classNames[classId] + ": " + cv::format("%.2f", confidence);
        else
            label = "Unknown: " + cv::format("%.2f", confidence);

        int baseline;
        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);

        int labelX = max(x, 0);
        int labelY = max(y - 5, labelSize.height + 2);

        rectangle(frame, Point(labelX, labelY - labelSize.height - 4),
                  Point(labelX + labelSize.width, labelY), Scalar(0, 255, 0), -1);

        putText(frame, label, Point(labelX, labelY - 2), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
    }
}
