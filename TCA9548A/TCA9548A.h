#ifndef TCA9548A_H
#define TCA9548A_H

#include <Wire.h>

class TCA9548A {
public:
    TCA9548A(uint8_t address);
    void begin();
    void enableChannels(uint8_t channels);
    void disableChannels(uint8_t channels);
private:
    uint8_t _address;
};

#endif
