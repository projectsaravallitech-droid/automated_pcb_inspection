#include <opencv2/opencv.hpp>
using namespace cv;

int main() {
    // Open default webcam (0 = first camera)
    VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cout << "Error: Cannot open webcam\n";
        return -1;
    }

    Mat frame;

    while (true) {
        cap.read(frame);  // Capture frame

        if (frame.empty()) {
            std::cout << "Error: Blank frame grabbed\n";
            break;
        }

        imshow("USB Webcam Stream", frame);

        // Press 'q' to exit
        if (waitKey(1) == 'q')
            break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
