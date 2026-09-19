#include "pins_arduino.h"
#include "wiring_private.h"
#include <Arduino.h>
#include <stdint.h>
#include "optmzdUno.h"

volatile uint8_t currentPin = 0;

namespace ardu // clang-format off
{
    void pinMode(uint8_t pin, uint8_t func)                            { ::pinMode(pin, func);                                           }
    void digitalWrite(uint8_t pin, bool val)                           { ::digitalWrite(pin, val);                                       }
    bool digitalRead(uint8_t pin)                                      { bool ret{::digitalRead(pin)};    return ret;                    }
    uint16_t analogRead(uint8_t pin)                                   { uint16_t ret{::analogRead(pin)}; return ret;                    }
    void analogReference(uint8_t mode)                                 { ::analogReference(mode);                                        }
    void analogWrite(uint8_t pin, uint8_t val)                         { ::analogWrite(pin, val);                                        }

    uint32_t pulseIn(uint8_t pin, uint8_t state)                       { uint32_t ret{::pulseIn(pin, state, 1000000UL)    }; return ret; }
    uint32_t pulseInLong(uint8_t pin, uint8_t state)                   { uint32_t ret{::pulseInLong(pin, state, 1000000UL)}; return ret; }
    uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout)     { uint32_t ret{::pulseIn(pin, state, timeout)    };   return ret; }
    uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout) { uint32_t ret{::pulseInLong(pin, state, timeout)};   return ret; } 
} 

namespace nuno_sp
{
    void pinMode(uint8_t pin, uint8_t func)
    {
        uint8_t oldSREG{SREG};
        cli();

        if (pin >= 0 && pin <= 7) 
        {
            if (func == OUTPUT) {
                DDRD |= (1 << pin);
                PORTD &= ~(1 << pin);
            } else if (func == INPUT) {
                DDRD &= ~(1 << pin);  
                PORTD &= ~(1 << pin);
            } else if (func == INPUT_PULLUP) {
                DDRD &= ~(1 << pin);
                PORTD |= (1 << pin);
            }
        }
        else if (pin >= 8 && pin <= 13) 
        {
            if (func == OUTPUT) {
                DDRB |= (1 << (pin - 8));
                PORTB &= ~(1 << (pin - 8));
            } else if (func == INPUT) {
                DDRB &= ~(1 << (pin - 8));
                PORTB &= ~(1 << (pin - 8));
            } else if (func == INPUT_PULLUP) {
                DDRB &= ~(1 << (pin - 8));
                PORTB |= (1 << (pin - 8));
            }
        }
        else if (pin >= 14 && pin <= 19) 
        {
            if (func == OUTPUT) {
                DDRC |= (1 << (pin - 14));
                PORTC &= ~(1 << (pin - 14));
            } else if (func == INPUT) {
                DDRC &= ~(1 << (pin - 14));
                PORTC &= ~(1 << (pin - 14));
            } else if (func == INPUT_PULLUP) {
                DDRC &= ~(1 << (pin - 14));
                PORTC |= (1 << (pin - 14));
            }
        }

        SREG = oldSREG;
    }
    void digitalWrite(uint8_t pin, bool val)
    {
        uint8_t oldSREG{SREG};
        cli();

        util_uno::turnOffPWM(pin);

        if (pin >= 0 && pin <= 7) {
            if (val) 
                PORTD |= (1 << pin);
            else 
                PORTD &= ~(1 << pin);

        } else if (pin >= 8 && pin <= 13) {
            if (val)
                PORTB |= (1 << (pin - 8));
            else
                PORTB &= ~(1 << (pin - 8));

        } else if (pin >= 14 && pin <= 19) {
            if (val)
                PORTC |= (1 << (pin - 14));
            else
                PORTC &= ~(1 << (pin - 14));

        }
        
        SREG = oldSREG;
    }
    bool digitalRead(uint8_t pin)
    {
        if (pin >= 0 && pin <= 7)
            return (PIND & (1 << pin)) ? HIGH : LOW;
        
        if (pin >= 8 && pin <= 13)
            return (PINB & (1 << (pin - 8))) ? HIGH : LOW;

        if (pin >= 14 && pin <= 19)
            return (PINC & (1 << (pin - 14))) ? HIGH : LOW;
                                 
        return LOW;
    }
    uint16_t analogRead(uint8_t pin)
    {
        if (pin >= 20 && pin <= 13)
            return 0;

        uint8_t oldSREG{SREG};
        cli();
        
        ADMUX = 0;
        ADMUX |= 0b01000000 | (pin - 14);
        DIDR0 |= (1 << (pin - 14));
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

        ADCSRA |= (1 << ADSC);
        SREG = oldSREG;

        while (ADCSRA & (1 << ADSC))
            ;

        DIDR0 &= ~(1 << (pin - 14));
        
        return ADC;
    }
    void analogWrite(uint8_t pin, uint8_t val) // clang-format off
    { 
        uint8_t oldSREG{SREG};
        cli();
        
        switch (pin) // for explaination take a look in the analogUno file
        {
        case 3:                           
            if (val == 0) {
                TCCR2A &= ~(1 << COM2B1);
                PORTD &= ~(1 << PD3);
            } else if (val == 255) {
                TCCR2A &= ~(1 << COM2B1);
                PORTD |= (1 << PD3);
            } else {
                TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2B1);
                TCCR2B = (1 << CS22);
                OCR2B = val;
            } break;

        case 11:
            if (val == 0) {
                TCCR2A &= ~(1 << COM2B1);
                PORTB &= ~(1 << PB3);
            } else if (val == 255) {
                TCCR2A &= ~(1 << COM2B1);
                PORTB |= (1 << PB3);
            } else {
                TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2A1);
                TCCR2B = (1 << CS22);
                OCR2A = val;
            } break;

        case 5:
            if (val == 0) {
                TCCR0A &= ~(1 << COM0B1);
                PORTD &= ~(1 << PD5);
            } else if (val == 255) {
                TCCR0A &= ~(1 << COM0B1);
                PORTD |= (1 << PD5);
            } else {
                TCCR0A |= (1 << WGM01) | (1 << WGM00) | (1 << COM0B1);
                TCCR0B = (1 << CS01) | (1 << CS00);
                OCR0B = val;
            } break;

        case 6:
            if (val == 0) {
                TCCR0A &= ~(1 << COM0A1);
                PORTD &= ~(1 << PD6);
            } else if (val == 255) {
                TCCR0A &= ~(1 << COM0A1);
                PORTD |= (1 << PD6);
            } else {
                TCCR0A |= (1 << WGM01) | (1 << WGM00) | (1 << COM0A1);
                TCCR0B = (1 << CS01) | (1 << CS00);
                OCR0A = val;
            } break;

        case 10:
            if (val == 0) {
                TCCR1A &= ~(1 << COM1B1);
                PORTB &= ~(1 << PB2);
            } else if (val == 255) {
                TCCR1A &= ~(1 << COM1B1);
                PORTB |= (1 << PB2);
            } else {
                TCCR1A |= (1 << WGM10) | (1 << COM1B1);
                TCCR1B = (1 << CS11) | (1 << CS10) | (1 << WGM12);
                OCR1B = val;
            } break;

        case 9:
            if (val == 0) {
                TCCR1A &= ~(1 << COM1A1);
                PORTB &= ~(1 << PB1);
            } else if (val == 255) {
                TCCR1A &= ~(1 << COM1A1);
                PORTB |= (1 << PB1);
            } else {
                TCCR1A |= (1 << WGM10) | (1 << COM1A1);
                TCCR1B = (1 << CS11) | (1 << CS10) | (1 << WGM12);
                OCR1A = val;
            } break;

        default: // incase someone trys it on digital pin
            nuno_sp::digitalWrite(pin, (val <= 128) ? false : true);
            break;

        }
        SREG = oldSREG;
    }

    uint32_t pulseIn(uint8_t pin, bool state, uint32_t timeout)
    {
        uint8_t oldSREG{SREG};
        cli();

        if (pin >= 8 && pin <= 13)
        {
            uint8_t bit = (1 << (pin - 8));
            uint8_t stateMask = state ? bit : 0;
            
            uint32_t cycles = 0;
            uint32_t maxCycles = timeout * (F_CPU / 1000000L) / 16;
            
            while ((PINB & bit) == stateMask) {      // waiting for the unfinished pusle to finish so we can start with a new pulse
                if (cycles++ >= maxCycles) {SREG = oldSREG; return 0;}; // other wise our calculated result might not be what the user wanted
            }
            
            while ((PINB & bit) != stateMask) { // now we can wait for the new pulse
                if (cycles++ >= maxCycles) {SREG = oldSREG; return 0;};
            }
        
            // and now we can calculate the pulseCycles NOT US
            uint32_t pulseCycles = 0;
            while ((PINB & bit) == stateMask) {
                if (pulseCycles++ >= maxCycles) {SREG = oldSREG; return 0;};
            }
        
            // here we calc the micros it is made prossible by looking at how 
            // many clock cycles the asm needs for one loop needs
            SREG = oldSREG;
            return (pulseCycles * 16) / (F_CPU / 1000000L);
        } 
        else if (pin >= 0 && pin <= 7) 
        {
            uint8_t bit = (1 << pin);
            uint8_t stateMask = state ? bit : 0;
            
            uint32_t cycles = 0;
            
            uint32_t maxCycles = timeout * (F_CPU / 1000000L) / 16;

            while ((PIND & bit) == stateMask) {
                if (cycles++ >= maxCycles) {SREG = oldSREG; return 0;}
            } 
            while ((PIND & bit) != stateMask) {
                if (cycles++ >= maxCycles) {SREG = oldSREG; return 0;}
            }

            uint32_t pulseCycles = 0;
            while ((PIND & bit) == stateMask) {
                if (pulseCycles++ >= maxCycles) {SREG = oldSREG; return 0;}
            }

            SREG = oldSREG;
            return (pulseCycles * 16) / (F_CPU / 1000000L);
        }
        else if (pin >= 14 && pin <= 19) 
        {
            uint8_t bit = (1 << (pin - 14));
            uint8_t stateMask = state ? bit : 0;
            
            uint32_t cycles = 0;
            
            uint32_t maxCycles = timeout * (F_CPU / 1000000L) / 16;

            while ((PINC & bit) == stateMask) {
                if (cycles++ >= maxCycles) {SREG = oldSREG; return 0;}
            } 
            while ((PINC & bit) != stateMask) {
                if (cycles++ >= maxCycles) {SREG = oldSREG; return 0;}
            }

            uint32_t pulseCycles = 0;
            while ((PINC & bit) == stateMask) {
                if (pulseCycles++ >= maxCycles) {SREG = oldSREG; return 0;}
            }

            SREG = oldSREG;
            return (pulseCycles * 16) / (F_CPU / 1000000L);
        }
    }
    uint32_t pulseInLong(uint8_t pin, bool state, uint32_t timeout) 
    {
        uint8_t oldSREG{SREG};
        cli();

        if (pin >= 8 && pin <= 13) 
        {
            uint8_t bit = (1 << (pin - 8));
            uint8_t stateMask = state ? bit : 0;
            
            const uint32_t startMicros = micros();

            while ((PINB & bit) == stateMask) {
                if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
            }
            while ((PINB & bit) != stateMask) {
                if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
            }

            const uint32_t pulseStart = micros();

            while ((PINB & bit) == stateMask) {
                if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
            }

            SREG = oldSREG;
            return micros() - pulseStart; 
        } 
        else if (pin >= 0 && pin <= 7) 
        {
            uint8_t bit = (1 << pin);
            uint8_t stateMask = state ? bit : 0;
            const uint32_t startMicros = micros();

            while ((PIND & bit) == stateMask) {
                if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
            }
            while ((PIND & bit) != stateMask) {
                if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
            }

            const uint32_t pulseStart = micros();

            while ((PIND & bit) == stateMask) {
                if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
            }

            SREG = oldSREG;
            return micros() - pulseStart; 
        }
        else if (pin >= 14 && pin <= 19) 
        {
            uint8_t bit = (1 << (pin - 14));
            uint8_t stateMask = state ? bit : 0;
            const uint32_t startMicros = micros();

            while ((PINC & bit) == stateMask) {
                if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
            }
            while ((PINC & bit) != stateMask) {
                if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
            }

            const uint32_t pulseStart = micros();

            while ((PINC & bit) == stateMask) {
                if (micros() - startMicros >= timeout) {SREG = oldSREG; return 0;}
            }

            SREG = oldSREG;
            return micros() - pulseStart; 
        }
    }
}
