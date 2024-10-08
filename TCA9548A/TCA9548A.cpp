#include "TCA9548A.h"

TCA9548A::TCA9548A(uint8_t address) {
    _address = address;
}

void TCA9548A::begin() {
    Wire.begin();
}

void TCA9548A::enableChannels(uint8_t channels) {
    Wire.beginTransmission(_address);
    Wire.write(channels);
    Wire.endTransmission();
}

void TCA9548A::disableChannels(uint8_t channels) {
    Wire.beginTransmission(_address);
    Wire.write(0);
    Wire.endTransmission();
}
