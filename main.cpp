#include "VideoStream.h"
#include "UI.h"

int main() {
    VideoStream stream;
    UI ui(stream, "images.png");

    stream.start(0);
    ui.run();
    ui.wait();

    stream.stop();
    return 0;
}
