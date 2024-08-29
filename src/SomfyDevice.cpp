#include "SomfyDevice.h"

void SomfyDevice::init()
{
    eventManager->debug("Init Somfy device", 1);
    pinMode(emitterPin, OUTPUT);

    // to remove:
    uint16_t newCode = 40;
    config.setPreference(id + "_rcode", newCode);
    eventManager->debug("Rolling code reseted to "  + newCode, 1);
}

bool SomfyDevice::processCommand(String command, std::vector<String> params)
{
    if (command == "Up" || command == "Down" || command == "Stop" || command == "Prog") {
        eventManager->debug("Somfy command received: " + command, 1);
        sendSomfyCommand(getSomfyCommand(command));
        return true;
    }
    return false;
}

uint16_t SomfyDevice::nextCode()
{
    uint16_t code = config.getPreference(id+ "_rcode", 1);
    eventManager->debug("Rolling code: " + String(code), 2);
    config.setPreference(id + "_rcode", code + 1);
    return code;
}

void SomfyDevice::sendSomfyCommand(SomfyCommand command, int repeat) {
    eventManager->debug("Sending Somfy command: " + String(static_cast<byte>(command), HEX), 3);
	const uint16_t rollingCode = nextCode();
	byte frame[7];
	buildFrame(frame, command, rollingCode);
	sendFrame(frame, 2);
	for (int i = 0; i < repeat; i++) {
		sendFrame(frame, 7);
	}
}

void SomfyDevice::printFrame(byte *frame) {
	for (byte i = 0; i < 7; i++) {
		if (frame[i] >> 4 == 0) {  //  Displays leading zero in case the most significant
			Serial.print("0");     // nibble is a 0.
		}
		Serial.print(frame[i], HEX);
		Serial.print(" ");
	}
	Serial.println();
}

String SomfyDevice::getFrameString(byte *frame) {
    String result = "";
    for (byte i = 0; i < 7; i++) {
        if (frame[i] >> 4 == 0) {  //  Displays leading zero in case the most significant
            result += "0";     // nibble is a 0.
        }
        result += String(frame[i], HEX);
        result += " ";
    }
    return result;
}

void SomfyDevice::buildFrame(byte *frame, SomfyCommand command, uint16_t code) {
	const byte button = static_cast<byte>(command);
	frame[0] = 0xA7;          // Encryption key. Doesn't matter much
	frame[1] = button << 4;   // Which button did  you press? The 4 LSB will be the checksum
	frame[2] = code >> 8;     // Rolling code (big endian)
	frame[3] = code;          // Rolling code
	frame[4] = remote >> 16;  // Remote address
	frame[5] = remote >> 8;   // Remote address
	frame[6] = remote;        // Remote address

#ifdef DEBUG
	Serial.print("Frame         : ");
	printFrame(frame);
#endif
    eventManager->debug("Frame         : " + getFrameString(frame), 3);

	// Checksum calculation: a XOR of all the nibbles
	byte checksum = 0;
	for (byte i = 0; i < 7; i++) {
		checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
	}
	checksum &= 0b1111;  // We keep the last 4 bits only

	// Checksum integration
	frame[1] |= checksum;

#ifdef DEBUG
	Serial.print("With checksum : ");
	printFrame(frame);
#endif
    eventManager->debug("With checksum : " + getFrameString(frame), 3);

	// Obfuscation: a XOR of all the bytes
	for (byte i = 1; i < 7; i++) {
		frame[i] ^= frame[i - 1];
	}

#ifdef DEBUG
	Serial.print("Obfuscated    : ");
	printFrame(frame);
#endif
    eventManager->debug("Obfuscated    : " + getFrameString(frame), 3);
}

void SomfyDevice::sendFrame(byte *frame, byte sync) {
	if (sync == 2) {  // Only with the first frame.
		// Wake-up pulse & Silence
		sendHigh(9415);
		sendLow(9565);
		delay(80);
	}

	// Hardware sync: two sync for the first frame, seven for the following ones.
	for (int i = 0; i < sync; i++) {
		sendHigh(4 * SYMBOL);
		sendLow(4 * SYMBOL);
	}

	// Software sync
	sendHigh(4550);
	sendLow(SYMBOL);

	// Data: bits are sent one by one, starting with the MSB.
	for (byte i = 0; i < 56; i++) {
		if (((frame[i / 8] >> (7 - (i % 8))) & 1) == 1) {
			sendLow(SYMBOL);
			sendHigh(SYMBOL);
		} else {
			sendHigh(SYMBOL);
			sendLow(SYMBOL);
		}
	}

	// Inter-frame silence
	sendLow(415);
	delay(30);
}

void SomfyDevice::sendHigh(uint16_t durationInMicroseconds) {
#if defined(ESP32) || defined(ESP8266)
	digitalWrite(emitterPin, HIGH);
	delayMicroseconds(durationInMicroseconds);
#elif defined(ARDUINO_ARCH_AVR)
	// TODO fast write
	digitalWrite(emitterPin, HIGH);
	delayMicroseconds(durationInMicroseconds);
#endif
}

void SomfyDevice::sendLow(uint16_t durationInMicroseconds) {
#if defined(ESP32) || defined(ESP8266)
	digitalWrite(emitterPin, LOW);
	delayMicroseconds(durationInMicroseconds);
#elif defined(ARDUINO_ARCH_AVR)
	// TODO fast write
	digitalWrite(emitterPin, LOW);
	delayMicroseconds(durationInMicroseconds);
#endif
}

SomfyCommand SomfyDevice::getSomfyCommand(const String &string) {
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