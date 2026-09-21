// clang-format off
/*
'                        ___                             ___   __   __                     
'  _______   _______   _|   |_   _______   _______   ___|   | |  | |  |  _ _____   _______  fast register Library (Uno/nano)
' |       | |       | |_     _| |       | |___    | |       | |  | |  | | V _   | |       | Version 1.1.0
' |   '   | |   '   |   |   |   |  |  | | |   ____| |   '   | |  |_|  | |  | |  | |   '   | Copyright (c) 2026 Tim
' |_______| |    ___|   |___|   |__|__|_| |_______| |_______| |_______| |__| |__| |_______| All right reserved.
'           |___|

  this libary is a Sub-lib of optmzdArd.h without timer functions such as millis() delay() ...
  and ONLY nano/uno namespace if you want to use SensorRobot with optmzdArd use the SensorRobot_sp version
  it is build with optmzdArd in mind and all the timer functions are straight out of opmtzdArd
  if you wanna know more about the registers of Uno etz take a look over in 
  https://github.com/cvmDestroyer/optmzdArd/tree/main/lib/optmzdArd/src
*/
#ifndef optmzd_Uno_hpp
#define optmzd_Uno_hpp

#include <Arduino.h>
#include <stdint.h>
#include "utilUno.h"

namespace ardu
{
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, bool val);
    bool digitalRead(uint8_t pin);
    uint16_t analogRead(uint8_t pin);
    void analogWrite(uint8_t pin, uint8_t val);

    uint32_t pulseIn(uint8_t pin, uint8_t state);
    uint32_t pulseInLong(uint8_t pin, uint8_t state);
    uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout);
    uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout);

    template<uint8_t PIN> void pinMode(uint8_t func)    { ::pinMode(PIN, func);                        }
    template<uint8_t PIN> void digitalWrite(bool val)   { ::digitalWrite(PIN, val);                    }
    template<uint8_t PIN> bool digitalRead(void)        { bool ret{::digitalRead(PIN)};    return ret; }
    template<uint8_t PIN> uint16_t analogRead(void)     { uint16_t ret{::analogRead(PIN)}; return ret; }
    template<uint8_t PIN> void analogWrite(uint8_t val) { ::analogWrite(PIN, val);                     }
    template<uint8_t PIN> uint32_t pulseIn(uint8_t state)                       { uint32_t ret{::pulseIn(PIN, state, 1000000UL)    }; return ret; }
    template<uint8_t PIN> uint32_t pulseInLong(uint8_t state)                   { uint32_t ret{::pulseInLong(PIN, state, 1000000UL)}; return ret; }
    template<uint8_t PIN> uint32_t pulseIn(uint8_t state, uint32_t timeout)     { uint32_t ret{::pulseIn(PIN, state, timeout)    };   return ret; }
    template<uint8_t PIN> uint32_t pulseInLong(uint8_t state, uint32_t timeout) { uint32_t ret{::pulseInLong(PIN, state, timeout)};   return ret; }
}

namespace nuno_sp
{
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, bool val);
    bool digitalRead(uint8_t pin);
    uint16_t analogRead(uint8_t pin);
    void analogWrite(uint8_t pin, uint8_t val);

    uint32_t pulseIn(uint8_t pin, bool state, uint32_t timeout);
    uint32_t pulseInLong(uint8_t pin, bool state, uint32_t timeout);
    uint32_t pulseIn(uint8_t pin, bool state, uint32_t timeout = 1000000L);
    uint32_t pulseInLong(uint8_t pin, bool state, uint32_t timeout = 1000000L);
    
    template<uint8_t PIN> struct hardwearLvl;
    #define DEFINE_PIN(pin, port_letter, port_bit, adc_ch, is_pwm, pwm_reg, tccra_reg, tccrb_reg, com_bit, tccra_bits, tccrb_bits) \
    template<> struct hardwearLvl<pin> { \
        static constexpr uintptr_t PORT       = (uintptr_t)&PORT##port_letter; \
        static constexpr uintptr_t PIN_REG    = (uintptr_t)&PIN##port_letter; \
        static constexpr uintptr_t DDR        = (uintptr_t)&DDR##port_letter; \
        static constexpr uint8_t   BIT        = port_bit; \
        static constexpr uint8_t   ADC_CH     = adc_ch; \
        static constexpr bool      HAS_PWM    = is_pwm; \
        static constexpr uintptr_t PWM_REG    = (uintptr_t)(pwm_reg); \
        static constexpr uintptr_t TCCRA_REG  = (uintptr_t)(tccra_reg); \
        static constexpr uintptr_t TCCRB_REG  = (uintptr_t)(tccrb_reg); \
        static constexpr uint8_t   COM_BIT    = com_bit; \
        static constexpr uint8_t   TCCRA_BITS = tccra_bits; \
        static constexpr uint8_t   TCCRB_BITS = tccrb_bits; \
    };

    // ---------------------------------------------------------------------------------------
    //         pin  reg  bit  adc   pwm      OCR      TCCRA     TCCRB        COM     tccra bits  tccrb bits
    DEFINE_PIN( 0,   D,   0,  255, false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 1,   D,   1,  255, false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 2,   D,   2,  255, false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 3,   D,   3,  255, true ,  &OCR2B,  &TCCR2A,  &TCCR2B,  (1 << COM2B1),  0x03,       0x04   )
    DEFINE_PIN( 4,   D,   4,  255, false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 5,   D,   5,  255, true ,  &OCR0B,  &TCCR0A,  &TCCR0B,  (1 << COM0B1),  0x03,       0x03   )
    DEFINE_PIN( 6,   D,   6,  255, true ,  &OCR0A,  &TCCR0A,  &TCCR0B,  (1 << COM0A1),  0x03,       0x03   )
    DEFINE_PIN( 7,   D,   7,  255, false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 8,   B,   0,  255, false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 9,   B,   1,  255, true ,  &OCR1A,  &TCCR1A,  &TCCR1B,  (1 << COM1A1),  0x01,       0x0B   )
    DEFINE_PIN( 10,  B,   2,  255, true ,  &OCR1B,  &TCCR1A,  &TCCR1B,  (1 << COM1B1),  0x01,       0x0B   )
    DEFINE_PIN( 11,  B,   3,  255, true ,  &OCR2A,  &TCCR2A,  &TCCR2B,  (1 << COM2A1),  0x03,       0x04   )
    DEFINE_PIN( 12,  B,   4,  255, false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 13,  B,   5,  255, false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 14,  C,   0,   0,  false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 15,  C,   1,   1,  false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 16,  C,   2,   2,  false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 17,  C,   3,   3,  false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 18,  C,   4,   4,  false,     0,        0,        0,          0,         0,          0     )
    DEFINE_PIN( 19,  C,   5,   5,  false,     0,        0,        0,          0,         0,          0     )
    
    template<uint8_t PIN, bool HAS_PWM = hardwearLvl<PIN>::HAS_PWM> // these are for anlogRead cuz
    struct analogWriteHelper {                                      // if constexpr only works cince C++ 17 and 
        static void apply(uint8_t val) {                            // avr-compiler are stuck on C++ 11
            digitalWrite<PIN>(val >= 128);
        }
    };

    template<uint8_t PIN>
    struct analogWriteHelper<PIN, true> {
        // i feel like this doesnt need explaination its in normal analogWrite if you want it
        static void apply(uint8_t val) {
            if (val == 0) {
                *reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::TCCRA_REG) &= ~hardwearLvl<PIN>::COM_BIT;
                digitalWrite<PIN>(LOW);
            }
            else if (val == 255) {
                *reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::TCCRA_REG) &= ~hardwearLvl<PIN>::COM_BIT;
                digitalWrite<PIN>(HIGH);
            }
            else {
                *reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::TCCRA_REG) |= hardwearLvl<PIN>::COM_BIT | hardwearLvl<PIN>::TCCRA_BITS;
                *reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::TCCRB_REG) = hardwearLvl<PIN>::TCCRB_BITS;
                *reinterpret_cast<volatile uint16_t*>(hardwearLvl<PIN>::PWM_REG) = val;
            }
        }
    };

    // templates
    template<uint8_t PIN> void pinMode(uint8_t func) 
    {
        uint8_t oldSREG{SREG};
        cli();

        volatile uint8_t* outputReg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::DDR)};
        volatile uint8_t* pullupReg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PORT)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};

        if (func == OUTPUT) {
           *outputReg |= mask;
           *pullupReg &= ~mask;
        } else if (func == INPUT_PULLUP) {
           *outputReg &= ~mask;
           *pullupReg |= mask;
        } else if (func == INPUT) {
           *outputReg &= ~mask;
           *pullupReg &= ~mask;
        }

        SREG = oldSREG;
    }
    template<uint8_t PIN> void digitalWrite(bool val) 
    {
        uint8_t oldSREG{SREG};
        cli();

        util_nuno_sp::template_util_sp<PIN>::turnOffPWM();
        
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PORT)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};
    
        if (val)
            *reg |= mask;
        else
            *reg &= ~mask;

        SREG = oldSREG;
    }
    template<uint8_t PIN> bool digitalRead(void) 
    {
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PIN_REG)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};

        return (*reg & mask);
    }
    template<uint8_t PIN> uint16_t analogRead(void) 
    {
        static_assert(hardwearLvl<PIN>::ADC_CH != 255, "ERROR: analogRead can only read anlog pins PWM DOES NOT COUNT(~pin) onyl A0 - A5");
        uint8_t oldSREG{SREG};
        cli();
        constexpr uint8_t channel{hardwearLvl<PIN>::ADC_CH};
        constexpr uint8_t bit{hardwearLvl<PIN>::BIT};
        
        ADMUX = 0;
        ADMUX |= 0b01000000 | channel;
        DIDR0 |= (1 << bit);
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
        
        ADCSRA |= (1 << ADSC);
        SREG = oldSREG;
        
        while (ADCSRA & (1 << ADSC))
            ; // null statement

        DIDR0 &= ~(1 << bit);
        
        return ADC;
    }
    template<uint8_t PIN> void analogWrite(uint8_t val) {
        analogWriteHelper<PIN>::apply(val);
    }
    template<uint8_t PIN> uint32_t pulseIn(bool state, uint32_t timeout) 
    {  
        uint8_t oldSREG{SREG};
        cli();
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PIN_REG)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};

        uint32_t cycles = 0;
            
        uint32_t maxCycles = timeout * (F_CPU / 1000000L) / 16;

        while ((*reg & mask) == (state ? mask : 0)) {
            if (cycles++ >= maxCycles) {SREG = oldSREG; return 0;}
        } 
        while ((*reg & mask) != (state ? mask : 0)) {
            if (cycles++ >= maxCycles) {SREG = oldSREG; return 0;}
        }
    
        uint32_t pulseCycles = 0;
        while ((*reg & mask) == (state ? mask : 0)) {
            if (pulseCycles++ >= maxCycles) {SREG = oldSREG; return 0;}
        }
    
        SREG = oldSREG;
        return (pulseCycles * 16) / (F_CPU / 1000000L);
    }  
    template<uint8_t PIN> uint32_t pulseInLong(bool state, uint32_t timeout) 
    {
        uint8_t oldSREG{SREG};
        cli();
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PIN_REG)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};
        
        const uint32_t startMicros = micros();

        while ((*reg & mask) == (state ? mask : 0)) {
            if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
        }
        while ((*reg & mask) != (state ? mask : 0)) {
            if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
        }

        const uint32_t pulseStart = micros();

        while ((*reg & mask) == (state ? mask : 0)) {
            if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
        }

        SREG = oldSREG;
        return micros() - pulseStart;
    }
    template<uint8_t PIN> uint32_t pulseIn(bool state) {
        return nuno_sp::pulseIn<PIN>(state, 1000000L);
    }
    template<uint8_t PIN> uint32_t pulseInLong(bool state) {
        return nuno_sp::pulseInLong<PIN>(state, 1000000UL);
    }
}

#endif