// clang-format off
#ifndef optmzd_Uno_hpp
#define optmzd_Uno_hpp

#include <Arduino.h>
#include <stdint.h>


constexpr uint8_t _INPUT         {0x0};
constexpr uint8_t _OUTPUT        {0x1};
constexpr uint8_t _INPUT_PULLUP  {0x2};

constexpr uint8_t _HIGH          {0x1};
constexpr uint8_t _LOW           {0x0};

constexpr bool performance      {true};

extern volatile uint8_t currentPin;

namespace ard
{
    void pinMode(uint8_t pin, uint8_t func);

    void digitalWrite(uint8_t pin, bool HILO);
    uint16_t digitalRead(uint8_t pin);

    uint16_t analogRead(uint8_t pin);
    void analogWrite(uint8_t pin, uint8_t uint);

    void tone(uint8_t pin, uint16_t frequency);
    void tone(uint8_t pin, uint16_t frequency, uint32_t duration);
    void noTone(uint8_t pin);
}

namespace uno
{
    void pinMode(uint8_t pin, uint8_t func);

    void digitalWrite(uint8_t pin, bool HILO);
    uint8_t digitalRead(uint8_t pin);

    uint16_t analogRead(uint8_t pin);
    uint16_t analogRead(uint8_t pin, bool modePeformance);
    void analogWrite(uint8_t pin, uint8_t uint);

    void tone(uint16_t frequency);
    void tone(uint8_t pin, uint16_t frequency);
    void noTone(uint8_t pin);
}

#endif