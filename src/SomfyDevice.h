#ifndef SOMFYDEVICE_H
#define SOMFYDEVICE_H

#include <Arduino.h>
#include <Configuration.h>
#include <Device.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <vector>
#include "SomfyRemote.h"

#define CC1101_FREQUENCY 433.42

enum class SomfyCommand : byte { My = 0x1, Up = 0x2, MyUp = 0x3, Down = 0x4, MyDown = 0x5, UpDown = 0x6, Prog = 0x8, SunFlag = 0x9, Flag = 0xA };

class SomfyDevice : public Device
{
  private:
  
    byte emitterPin = 2;

    uint32_t remoteStart = 0x100010;

    // collection of SomfyRemote objects
    std::vector<SomfyRemote> remotes;

    uint16_t getNextRollingCode(uint8_t num, bool save = true);

    SomfyCommand getSomfyCommand(const String& string);

  public:
    SomfyDevice(String id, Configuration& config, EventManager& eventMgr, byte emitterPin) : Device(id, config, eventMgr), emitterPin(emitterPin) {}

    void init() override;
    bool setRemoteStart(uint32_t remoteStart);

    bool processCommand(String command, std::vector<String> params) override;
    bool processMQTT(String topic, String value) override;

    int8_t addRemote(String name, String topic, uint8_t num = 0, uint32_t remoteAddr = 0);

    SomfyRemote* getRemote(uint8_t num);

    void sendSomfyCommand(uint8_t numRemote, SomfyCommand command, uint8_t repeat = 4);
};

#endif  // SOMFYDEVICE_H