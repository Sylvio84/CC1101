#include "MyController.h"

MyController::MyController(MyConfig& config)
    : MainController(config), myConfig(config), somfyDevice(new SomfyDevice("somfy", config, eventManager, EMITTER_GPIO))
{
    addDevice(somfyDevice);
}

void MyController::init()
{
    MainController::init();

    somfyDevice->init();
    int8_t num = somfyDevice->addRemote("Volet Salon Gauche", "volet/salon_gauche");
    eventManager.debug("Remote added: #" + String(num), 1);
}

void MyController::loop()
{
    MainController::loop();
}
