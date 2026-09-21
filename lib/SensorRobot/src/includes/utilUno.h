// clang-format off
/*                       ___
'              ___      |___|    ___       __   __                                 __
'  __   __   _|   |_   _____    |   |     |  | |  |  _ _____   _______            |  |____
' |  | |  | |_     _| |_    |   |   | __  |  | |  | | V _   | |       |           |   _   |
' |  |_|  |   |   |    _|   |_  |   v   | |  |_|  | |  | |  | |   '   |    ___    |  | |  |
' |_______|   |___|   |_______| \______7  |_______| |__| |__| |_______|   |___|   |__| |__|
'
  util libary for optmzdUno
*/

#include <Arduino.h>
#include <stdint.h>

#pragma once
#pragma once
#ifndef utilUno_h
#define utilUno_h

namespace util_nuno_sp
{
    static inline bool notPwmPin(uint8_t pin)
    {
        switch (pin) {
            case 3 : return 0;
            case 5 : return 0;
            case 6 : return 0;
            case 9 : return 0;
            case 10: return 0;
            case 11: return 0;
            default: return 1;
        }
    }

    inline void turnOffPWM(uint8_t pin)
    {
        if (notPwmPin(pin))
            return;

        switch (pin) {
            case 3 : TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0)); break;
            case 11: TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0)); break;
            case 10: TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0)); break;
            case 9 : TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0)); break;
            case 5 : TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0)); break;
            case 6 : TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0)); break;
        }
    }
    template <uint8_t PIN>
    struct template_util_sp
    {
        static bool notPwmPin()
        {
            switch (PIN) {
                case 3 : return 0;
                case 5 : return 0;
                case 6 : return 0;
                case 9 : return 0;
                case 10: return 0;
                case 11: return 0;
                default: return 1;
            }
        }
        static void turnOffPWM()
        {
            switch (PIN) {
            case 3 : TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0)); break;
            case 11: TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0)); break;
            case 10: TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0)); break;
            case 9 : TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0)); break;
            case 5 : TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0)); break;
            case 6 : TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0)); break;
        }
        }
    };

}

#endif