#include "SomfyDevice.h"

void SomfyDevice::init()
{
    eventManager->debug("Init Somfy device", 1);
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(CC1101_FREQUENCY);

    pinMode(emitterPin, OUTPUT);
    digitalWrite(emitterPin, LOW);

    setRemoteStart(this->remoteStart);
}

bool SomfyDevice::setRemoteStart(uint32_t remoteStart)
{
    if (remoteStart == 0 || remoteStart >= 0xFFFFFF) {
        return false;
    }

    this->remoteStart = remoteStart;
    config.setPreference(id + "_rstart", remoteStart);
    return true;
}

int8_t SomfyDevice::addRemote(String name, String topic, uint8_t num, uint32_t remoteAddr)
{
    if (num == 0) {
        num = remotes.size() + 1;
    }
    if (remoteAddr == 0) {
        remoteAddr = remoteStart + num - 1;
    }
    if (remoteAddr == 0 || remoteAddr >= 0xFFFFFF) {
        eventManager->debug("Remote address out of range: " + String(remoteAddr), 1);
        return -1;
    }
    remotes.push_back(SomfyRemote(emitterPin, name, topic, num, remoteAddr));

    eventManager->triggerEvent("mqtt", "subscribe", {topic});
    return num;
}

SomfyRemote* SomfyDevice::getRemote(uint8_t num)
{
    for (auto& remote : remotes) {
        if (remote.num == num) {
            return &remote;
        }
    }
    return nullptr;
}

void SomfyDevice::sendSomfyCommand(uint8_t numRemote, SomfyCommand command, uint8_t repeat)
{
    SomfyRemote* remote = getRemote(numRemote);
    if (remote == nullptr) {
        eventManager->debug("Remote not found: " + String(numRemote), 1);
        return;
    }

    ELECHOUSE_cc1101.SetTx();
    uint16_t rollingCode = getNextRollingCode(numRemote);
    byte button = static_cast<byte>(command);
    remote->sendCommand(button, rollingCode, repeat);
    ELECHOUSE_cc1101.setSidle();
}

uint16_t SomfyDevice::getNextRollingCode(uint8_t num, bool save)
{
    SomfyRemote* remote = getRemote(num);
    if (remote == nullptr) {
        eventManager->debug("Remote not found: " + String(num), 1);
        return -1;
    }
    String key = id + "_rc_" + String(remote->remote, HEX);
    uint16_t code = config.getPreference(key, 1);
    eventManager->debug("Rolling code: #" + key + " = " + String(code), 2);
    if (save) {
        config.setPreference(key, code + 1);
    }
    return code;
}

bool SomfyDevice::processCommand(String command, std::vector<String> params)
{
    if (command == "up" || command == "down" || command == "stop" || command == "prog") {
        if (params.size() == 0) {
            eventManager->debug("Somfy command requires a remote number", 1);
            return false;
        }
        uint8_t num = params[0].toInt();
        eventManager->debug("Somfy command received: " + command + " on #" + String(num), 1);
        sendSomfyCommand(num, getSomfyCommand(command));
        return true;
    } else if (command == "info") {
        if (params.size() == 1) {
            SomfyRemote* remote = getRemote(params[0].toInt());
            Serial.println("Num: " + String(remote->num));
            Serial.println("Name: " + remote->name);
            Serial.println("Topic: " + remote->topic);
            Serial.println("Remote: " + String(remote->remote, HEX));
        } else {
            for (auto& remote : remotes) {
                Serial.println("#" + String(remote.num) + " " + remote.name + " (" + remote.topic + ")");
            }
        }
        return true;
    } else if (command == "code") {
        if (params.size() == 1) {
            uint8_t num = params[0].toInt();
            uint16_t code = getNextRollingCode(num, false);
            Serial.println("Next Rolling code #" + String(num) + " = " + String(code));
        } else {
            for (auto& remote : remotes) {
                uint16_t code = getNextRollingCode(remote.num, false);
                Serial.println("Next Rolling code #" + String(remote.num) + " = " + String(code));
            }
        }
    }
    return false;
}

bool SomfyDevice::processMQTT(String topic, String value)
{
    eventManager->debug("Received MQTT message: " + topic + " = " + value, 2);
    for (auto& remote : remotes) {
        if (topic == remote.topic) {
            eventManager->debug("Topic matched: " + remote.topic, 2);
            sendSomfyCommand(remote.num, getSomfyCommand(value));
            return true;
        }
    }
    return false;
}

SomfyCommand SomfyDevice::getSomfyCommand(const String& string)
{
    if (string.equalsIgnoreCase("My")) {
        return SomfyCommand::My;
    } else if (string.equalsIgnoreCase("Up")) {
        return SomfyCommand::Up;
    } else if (string.equalsIgnoreCase("MyUp")) {
        return SomfyCommand::MyUp;
    } else if (string.equalsIgnoreCase("Down")) {
        return SomfyCommand::Down;
    } else if (string.equalsIgnoreCase("MyDown")) {
        return SomfyCommand::MyDown;
    } else if (string.equalsIgnoreCase("UpDown")) {
        return SomfyCommand::UpDown;
    } else if (string.equalsIgnoreCase("Prog")) {
        return SomfyCommand::Prog;
    } else if (string.equalsIgnoreCase("SunFlag")) {
        return SomfyCommand::SunFlag;
    } else if (string.equalsIgnoreCase("Flag")) {
        return SomfyCommand::Flag;
    } else if (string.length() == 1) {
        return static_cast<SomfyCommand>(strtol(string.c_str(), nullptr, 16));
    } else {
        return SomfyCommand::My;
    }
}