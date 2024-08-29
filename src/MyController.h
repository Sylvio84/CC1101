#ifndef MYCONTROLLER_H
#define MYCONTROLLER_H

#include <Arduino.h>
#include <Configuration.h>
#include <MainController.h>
//#include <NVSRollingCodeStorage.h>
//#include <EEPROM.h>
//#include <EEPROMRollingCodeStorage.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
//#include <SomfyRemote.h>
#include "SomfyDevice.h"
#include "config.h"

class MyController : public MainController
{
   private:
    MyConfig& myConfig;

    /*
    const uint8_t LED_PIN = 2;
    uint8_t wifiStatus = 0;
    */
    //NVSRollingCodeStorage rollingCodeStorage;
    /*EEPROMRollingCodeStorage rollingCodeStorage;
    SomfyRemote somfyRemote;*/
    SomfyDevice* somfyDevice;

   public:
    MyController(MyConfig& config);

    void init() override;
    void loop() override;

    /*void processEvent(String type, String action, std::vector<String> params) override;
    void processMQTT(String topic, String value) override;    */
    //void processCommand(String command, std::vector<String> params) override;

    //void sendCC1101Command(String cmd);
};

#endif
