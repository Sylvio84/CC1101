#include "MyController.h"

#define EMITTER_GPIO 2
//#define NVS_NAME "somfy"
//#define NVS_KEY "remote_1"
#define EEPROM_ADDRESS 0
#define REMOTE 0x5184c8

#define CC1101_FREQUENCY 433.42

// https://github.com/Legion2/Somfy_Remote_Lib/blob/main/examples/MultipleRemotes/MultipleRemotes.ino

MyController::MyController(MyConfig& config)
    //: MainController(config), myConfig(config), rollingCodeStorage(NVS_NAME, NVS_KEY), somfyRemote(EMITTER_GPIO, REMOTE, &rollingCodeStorage)
    : MainController(config), myConfig(config), rollingCodeStorage(EEPROM_ADDRESS), somfyRemote(EMITTER_GPIO, REMOTE, &rollingCodeStorage)
{}

void MyController::init()
{
    MainController::init();
    somfyRemote.setup();

    //ELECHOUSE_cc1101.setGDO(13, 12);
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(CC1101_FREQUENCY);
    // https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/README.md
    //ELECHOUSE_cc1101.setGDO(13, 12);
    //ELECHOUSE_cc1101.setSpiPin(18, 19, 23, 5);

#if defined(ESP32)
    if (!EEPROM.begin(4)) {
        Serial.println("failed to initialise EEPROM");
        delay(1000);
    }
#elif defined(ESP8266)
    EEPROM.begin(4);
#endif

    config.setPreference("debug_level", 3);
    config.setPreference("hostname", "SomfyRemote");
}

void MyController::loop()
{
    MainController::loop();
}

void MyController::sendCC1101Command(String cmd)
{
    ELECHOUSE_cc1101.SetTx();
    const Command command = getSomfyCommand(cmd);
    somfyRemote.sendCommand(command);
    ELECHOUSE_cc1101.setSidle();
    eventManager.debug("Command sent: " + cmd, 1);
}

void MyController::processCommand(String command, std::vector<String> params)
{
    MainController::processCommand(command, params);
    eventManager.debug("Command received: " + command, 1);

    if (command == "Up" || command == "Down" || command == "Stop" || command == "Prog") {
        eventManager.debug("Somfy command received", 1);
        sendCC1101Command(command);
    }
}

/*
void MyController::processEvent(String type, String action, std::vector<String> params)
{
    // Serial.println("Event received: " + type + " " + action);
    MainController::processEvent(type, action, params);
    if (type == "WiFi") {
        if (action == "Connected" || action == "ConnectionRecovered") {
            wifiStatus = 2;
        } else if (action == "ConnectionFailed" || action == "Disconnected" || action == "ConnectionLost") {
            wifiStatus = 0;
        } else if (action == "ConnectionInProgress") {
            wifiStatus = 1;
        } else if (action == "AccessPointStarted") {
            wifiStatus = 3;
        }
    }
}

void MyController::processMQTT(String topic, String value)
{
    // Serial.println("*** Received MQTT message *** " + topic + " = " + value);
    MainController::processMQTT(topic, value);
    String restartTopic = hostname + "/restart";
    if (topic == restartTopic && value == "1") {
        eventManager.debug("Restarting...", 1);
        ESP.restart();
    }
}
*/
