#ifndef SOMFYDEVICE_H
#define SOMFYDEVICE_H

#include <Arduino.h>
#include <Configuration.h>
#include <Device.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>

enum class SomfyCommand : byte {
	My = 0x1,
	Up = 0x2,
	MyUp = 0x3,
	Down = 0x4,
	MyDown = 0x5,
	UpDown = 0x6,
	Prog = 0x8,
	SunFlag = 0x9,
	Flag = 0xA
};


#define CC1101_FREQUENCY 433.42

class SomfyDevice : public Device
{
private:
    byte emitterPin = 2;

    uint32_t remote = 0;

	void buildFrame(byte *frame, SomfyCommand command, uint16_t code);
	void sendFrame(byte *frame, byte sync);
	void printFrame(byte *frame);
    String getFrameString(byte *frame);

	void sendHigh(uint16_t durationInMicroseconds);
	void sendLow(uint16_t durationInMicroseconds);

    uint16_t nextCode();

    SomfyCommand getSomfyCommand(const String &string);

    const uint16_t SYMBOL = 640;  // µs

public:

    SomfyDevice(String id, Configuration &config, EventManager &eventMgr, byte emitterPin, uint32_t remote) : Device(id, config, eventMgr)
    {
        this->emitterPin = emitterPin;
        this->remote = remote;
    };

    void init() override;

    bool processCommand(String command, std::vector<String> params) override;


    void sendSomfyCommand(SomfyCommand command, int repeat = 4);
    //void sendSomfyCommand(const String &string, int repeat = 4);

    
};
#endif