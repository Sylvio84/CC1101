#include <Arduino.h>
#include "config.h"
#include "MyController.h"

MyConfig config;
MyController myController(config);

void setup()
{
    myController.init();
}

void loop()
{
    myController.loop();
}
