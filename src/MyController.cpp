#include "MyController.h"

MyController::MyController(MyConfig& config)
    : MainController(config), myConfig(config), somfyDevice(new SomfyDevice("vr1", config, eventManager, EMITTER_GPIO, REMOTE))
{
    addDevice(somfyDevice);
}

void MyController::init()
{
    MainController::init();


    config.setPreference("debug_level", 3);
    config.setPreference("hostname", "SomfyRemote");
}

void MyController::loop()
{
    MainController::loop();
}
