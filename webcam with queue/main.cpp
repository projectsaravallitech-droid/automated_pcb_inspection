#include "VideoController.h"
#include "InspectionQueue.h"
#include "FrameQueue.h"
#include "Camera.h"
#include "Inspection.h"
#include "DisplayManager.h"
#include "AppContext.h"

int main()
{
    VideoController video;

    InspectionQueue inspectionQueue(5);
    FrameQueue displayQueue(5);

    Camera camera(&inspectionQueue);

    Inspection inspection(&inspectionQueue,
                          &displayQueue,
                          &video);

    AppContext context;
    context.controller = &video;
    context.camera = &camera;

    DisplayManager display(&displayQueue, &context);

    camera.start();
    inspection.start();
    display.start();

    std::cin.get();

    camera.stop();
    inspection.stop();
    display.stop();

    return 0;
}