#ifndef MYCONTROLLER_H
#define MYCONTROLLER_H

#include <Arduino.h>
#include <Configuration.h>
#include <MainController.h>
#include "SomfyDevice.h"
#include "config.h"

#define EMITTER_GPIO 2

class MyController : public MainController
{
   private:
    MyConfig& myConfig;
    SomfyDevice* somfyDevice;

   public:
    MyController(MyConfig& config);

    void init() override;
    void loop() override;

    /*
    void processEvent(String type, String action, std::vector<String> params) override;
    void processMQTT(String topic, String value) override;
    void processCommand(String command, std::vector<String> params) override;
    */

};

#endif
