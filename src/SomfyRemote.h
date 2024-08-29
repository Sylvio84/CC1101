#ifndef SOMFYREMOTE_H
#define SOMFYREMOTE_H

#include <Arduino.h>

class SomfyRemote
{
  private:
    void buildFrame(byte* frame, const byte button, uint16_t code);
    void sendFrame(byte* frame, byte sync);
    void printFrame(byte* frame);
    String getFrameString(byte* frame);

    void sendHigh(uint16_t durationInMicroseconds);
    void sendLow(uint16_t durationInMicroseconds);

    const uint16_t SYMBOL = 640;  // µs

  public:
    uint32_t remote = 0;
    uint8_t num;
    byte emitterPin;
    String name;
    String topic;

    SomfyRemote(byte emitterPin, String name, String topic, uint8_t num, uint32_t remoteAddr)
    {
        this->emitterPin = emitterPin;
        this->name = name;
        this->topic = topic;
        this->remote = remote;
        this->num = num;
    }

    void sendCommand(const byte button, uint16_t rollingCode, uint8_t repeat = 4);
};

#endif  // SOMFYREMOTE_H