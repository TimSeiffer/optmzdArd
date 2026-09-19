#include "pins_arduino.h"
#include "wiring_private.h"
#include <Arduino.h>
#include <stdint.h>
#include "optmzdArd.h"

volatile uint8_t currentPin = 0;

namespace ard // clang-format off
{
    void pinMode(uint8_t pin, uint8_t func) { ::pinMode(pin, func);}

    void digitalWrite(uint8_t pin, bool HILO) 
    {   // uint8_t timer = digitalPinToTimer(pin); uint8_t bit = digitalPinToBitMask(pin); uint8_t port = digitalPinToPort(pin); volatile uint8_t *out; if (port == NOT_A_PIN) return; if (timer != NOT_ON_TIMER) turnOffPWM(timer); out = portOutputRegister(port); uint8_t oldSREG = SREG; cli(); if (HILO == LOW) { *out &= ~bit; } else { *out |= bit; }  SREG = oldSREG;
        ::digitalWrite(pin, HILO);
    }
    uint16_t digitalRead(uint8_t pin) { uint16_t returnVal = ::digitalRead(pin); return returnVal; }

    uint16_t analogRead(uint8_t pin) { uint16_t returnVal = ::analogRead(pin); return returnVal; }
    void analogWrite(uint8_t pin, uint8_t uint) { ::analogWrite(pin, uint); }

    void tone(uint8_t pin, uint16_t frequency) { ::tone(pin, frequency); }
    void tone(uint8_t pin, uint16_t frequency, uint32_t duration) { ::tone(pin, frequency, duration); }
    void noTone(uint8_t pin) { ::noTone(pin); }
} 

namespace uno
{
    void pinMode(uint8_t pin, uint8_t func)
    {
        if (pin >= 0 && pin <= 7) 
        {
            if (func == _OUTPUT) {
                DDRD |= (1 << pin);
                PORTD &= ~(1 << pin);
            } else if (func == _INPUT) {
                DDRD &= ~(1 << pin);  
                PORTD &= ~(1 << pin);
            } else if (func == _INPUT_PULLUP) {
                DDRD &= ~(1 << pin);
                PORTD |= (1 << pin);
            }
        }
        else if (pin >= 8 && pin <= 13) 
        {
            uint8_t bit = pin - 8;
            if (func == _OUTPUT) {
                DDRB |= (1 << bit);
                PORTB &= ~(1 << bit);
            } else if (func == _INPUT) {
                DDRB &= ~(1 << bit);
                PORTB &= ~(1 << bit);
            } else if (func == _INPUT_PULLUP) {
                DDRB &= ~(1 << bit);
                PORTB |= (1 << bit);
            }
        }
    }

    void digitalWrite(uint8_t pin, bool HILO)
    {
        uint8_t oldSREG = SREG;
        cli();

        if (pin >= 0 && pin <= 7) 
        { 
            if (HILO) 
                PORTD |= (1 << pin);
            else 
                PORTD &= ~(1 << pin);
        }
        else if (pin >= 8 && pin <= 13) 
        {
            if (HILO)
                PORTB |= (1 << (pin - 8));
            else
                PORTB &= ~(1 << (pin - 8));
        }
        else if (pin >= 14 && pin <= 19) 
        {
            if (HILO)
                PORTB |= (1 << (pin - 14));
            else
                PORTB &= ~(1 << (pin - 14));
        }
        
        SREG = oldSREG;
    }
    uint8_t digitalRead(uint8_t pin)
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
        if (pin >= 14)
            pin -= 14;
        
        ADMUX = (1 << REFS0) | (pin & 0x07);

        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

        ADCSRA |= (1 << ADSC);

        while (ADCSRA & (1 << ADSC))
            ;

        return ADC;
    }
    uint16_t analogRead(uint8_t pin, bool modePeformance)
    {
        ADCSRA |= (1 << ADPS2);  // Bit 2 auf 1 setzen
        ADCSRA &= ~(1 << ADPS1); // Bit 1 auf 0 löschen
        ADCSRA &= ~(1 << ADPS0); // Bit 0 auf 0 löschen

        ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);

        ADCSRA |= (1 << ADSC);

        while (ADCSRA & (1 << ADSC))
            ;

        return ADC;
    }
    void analogWrite(uint8_t pin, uint8_t uint) // clang-format on
    {
        switch (pin)
        {

        case 3:
            if (uint == 0)
            {
                TCCR2A &= ~(1 << COM2B1);
                PORTD &= ~(1 << PORTD3);
            }
            else if (uint == 255)
            {
                TCCR2A &= ~(1 << COM2B1);
                PORTD |= (1 << PORTD3);
            }
            else
            {
                TCCR2A |= (1 << COM2B1);
                OCR2B = uint;
            }
            break;

        case 11:
            if (uint == 0)
            {
                TCCR2A &= ~(1 << COM2A1);
                PORTB &= ~(1 << PORTB3);
            }
            else if (uint == 255)
            {
                TCCR2A &= ~(1 << COM2A1);
                PORTB |= (1 << PORTB3);
            }
            else
            {
                TCCR2A |= (1 << COM2A1);
                OCR2A = uint;
            }
            break;

        case 5:
            if (uint == 0)
            {
                TCCR0A &= ~(1 << COM0B1);
                PORTD &= ~(1 << PORTD5);
            }
            else if (uint == 255)
            {
                TCCR0A &= ~(1 << COM0B1);
                PORTD |= (1 << PORTD5);
            }
            else
            {
                TCCR0A |= (1 << COM0B1);
                OCR0B = uint;
            }
            break;

        case 6:
            if (uint == 0)
            {
                TCCR0A &= ~(1 << COM0A1);
                PORTD &= ~(1 << PORTD6);
            }
            else if (uint == 255)
            {
                TCCR0A &= ~(1 << COM0A1);
                PORTD |= (1 << PORTD6);
            }
            else
            {
                TCCR0A |= (1 << COM0A1);
                OCR0A = uint;
            }
            break;

        case 9:
            if (uint == 0)
            {
                TCCR1A &= ~(1 << COM1A1);
                PORTB &= ~(1 << PORTB1);
            }
            else if (uint == 255)
            {
                TCCR1A &= ~(1 << COM1A1);
                PORTB |= (1 << PORTB1);
            }
            else
            {
                TCCR1A |= (1 << COM1A1);
                OCR1A = uint;
            }
            break;

        case 10:
            if (uint == 0)
            {
                TCCR1A &= ~(1 << COM1B1);
                PORTB &= ~(1 << PORTB2);
            }
            else if (uint == 255)
            {
                TCCR1A &= ~(1 << COM1B1);
                PORTB |= (1 << PORTB2);
            }
            else
            {
                TCCR1A |= (1 << COM1B1);
                OCR1B = uint;
            }
            break;

        default:
            bool HILO = true;
            if (uint <= 128)
                HILO = false;

            digitalWrite(pin, HILO);
            break;
        }
    }

    void tone(uint16_t frequency)
    {
        DDRD |= (1 << PB3);
        if (frequency < 61)
        {
            digitalWrite(3, LOW);
            return;
        }

        uint8_t prescalerBits = 0; // basicly speichert nur binar um genauer zu sein kann man auch 0b00000xxx
        uint32_t ocrValue = 0;

        if (frequency < 244)
        {
            ocrValue = (16000000UL / (1024UL * frequency)) - 1;
            prescalerBits = (1 << CS22) | (1 << CS21) | (1 << CS20); // prescaler 1024 0b00000110
        }
        else if (frequency < 488)
        {
            ocrValue = (16000000UL / (256UL * frequency)) - 1;
            prescalerBits = (1 << CS22) | (1 << CS21); // prescaler 256
        }
        else if (frequency < 976)
        {
            ocrValue = (16000000UL / (128UL * frequency)) - 1;
            prescalerBits = (1 << CS22) | (1 << CS20); // prescaler 128
        }
        else if (frequency < 1953)
        {
            ocrValue = (16000000UL / (64UL * frequency)) - 1;
            prescalerBits = (1 << CS22); // prescaler 64
        }
        else if (frequency < 7812)
        {
            ocrValue = (16000000UL / (32UL * frequency)) - 1;
            prescalerBits = (1 << CS21) | (1 << CS20); // prescaler 32
        }
        else
        {
            ocrValue = (16000000UL / (8UL * frequency)) - 1;
            prescalerBits = (1 << CS21); // prescaler 8
        }

        TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); // WGM22 ist in control-register b weil
        TCCR2B = (1 << WGM22) | prescalerBits;                // weil kein platz mehr in a war (würde im nachhinein hinzugefügt)
        // prescaler wird wieder geladen
        OCR2A = static_cast<uint8_t>(ocrValue);     // wie lange sie läuft (haben wir legit oben ausgerechnet du bastard)
        OCR2B = static_cast<uint8_t>(ocrValue / 2); // macht ne welle draus das halt auch funktioniert
    }
    void tone(uint8_t pin, uint16_t frequency)
    {
        currentPin = pin;
        pinMode(currentPin, OUTPUT);

        TCCR2A = 0;
        TCCR2B = 0;
        TCNT2 = 0;

        TCCR2A |= (1 << WGM21);

        TCCR2B |= (1 << CS22) | (1 << CS21);

        unsigned long ocrVal = (16000000 / (2 * 256 * frequency)) - 1;
        OCR2A = static_cast<uint8_t>(ocrVal);

        TIMSK2 |= (1 << OCIE2A);
    }
    void noTone(uint8_t pin)
    {
        TIMSK2 &= ~(1 << OCIE2A);
        digitalWrite(currentPin, LOW);
    }
}

ISR(TIMER2_COMPA_vect) { digitalWrite(currentPin, !digitalRead(currentPin)); } // clang-format on

// int main()
// {
//     uno::tone(8, 600);
//     ard::tone(8, 600);
//     uno::tone(8, 600);
//     ard::tone(8, 600);

//     uno::digitalWrite(5, HIGH);
//     ard::digitalWrite(5, HIGH);
//     uno::digitalRead(5);
//     ard::digitalRead(5);

//     uno::analogRead(5);
//     ard::analogRead(5);
// }
