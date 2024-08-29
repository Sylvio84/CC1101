#include "MyController.h"

#define EMITTER_GPIO 2
//#define REMOTE 0x100010
#define REMOTE 0x5184c8

#define CC1101_FREQUENCY 433.42

// https://github.com/Legion2/Somfy_Remote_Lib/blob/main/examples/MultipleRemotes/MultipleRemotes.ino

MyController::MyController(MyConfig& config)
    //: MainController(config), myConfig(config), rollingCodeStorage(NVS_NAME, NVS_KEY), somfyRemote(EMITTER_GPIO, REMOTE, &rollingCodeStorage)
    : MainController(config), myConfig(config), somfyDevice(new SomfyDevice("vr1", config, eventManager, EMITTER_GPIO, REMOTE))
{
    addDevice(somfyDevice);
}

void MyController::init()
{
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(CC1101_FREQUENCY);

    MainController::init();

    config.setPreference("debug_level", 3);
    config.setPreference("hostname", "SomfyRemote");
}

void MyController::loop()
{
    MainController::loop();
}
