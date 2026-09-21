#include <Arduino.h>
// #include "optmzdUno.h"
#include "SensorRobotV2.h"

#include <EEPROM.h>
#include <Servo.h>
#include <stdint.h>
#define __ 0

namespace uno = nuno_sp;

// ---------------------------------

struct pinConfig
{
    uint16_t servo : 4; // 1 uint16_t (unsigned int)
    uint16_t btn : 4;   //
    uint16_t echo : 4;  //
    uint16_t trig : 4;  //

    uint16_t riBa : 4; // 1 uint16_t (unsigned int)
    uint16_t leBa : 4; //
    uint16_t riFr : 4; //
    uint16_t leFr : 4; //

    uint8_t irLeft : 4;  // 1 uint8_t (unsigned Byte)
    uint8_t irRight : 4; //
};

const pinConfig pin{
    11, 12, 2, 9,
    3, 8, 5, 10,
    A0, A1};

Servo servo;
SensorRobot robot(pin.riBa, pin.leBa, pin.riFr, pin.leFr); // rb, lb, rf, lf

volatile uint32_t startTime{0};
volatile uint32_t duration{0};
volatile bool finishedCalc{false};

bool firstLoop = true;
uint16_t distance{999};

uint32_t timerLoop{0};
uint32_t timerRoute{0};
// bool pauseFor(uint32_t &previousTimeStamp, uint32_t waitTime);

void echoInterrupt();
// clang-format off
enum robotState { DRIVING, OBSTICAL_DETECTED, FINDING_NEW_ROUTE, LOST_TRACK };
// clang-format on
robotState currentState = DRIVING;

uint8_t irStatus[2] = {0, 0};
void irStatusSetter();

void setup()
{
    uno::pinMode(pin.irLeft, INPUT);
    uno::pinMode(pin.irRight, INPUT);

    uno::pinMode(pin.trig, OUTPUT);
    uno::pinMode(pin.echo, INPUT);

    uno::pinMode(pin.btn, INPUT_PULLUP);
    servo.attach(pin.servo);
    // no pinMode needed for motors cuz the lib does it already

    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(pin.echo), echoInterrupt, CHANGE);

    servo.write(90);
}

void loop()
{
    irStatusSetter();

    static uint32_t lastCalc = 0;
    if (millis() - lastCalc >= 500)
    {
        uno::digitalWrite(pin.trig, LOW);
        delayMicroseconds(2);
        uno::digitalWrite(pin.trig, HIGH);
        delayMicroseconds(10);
        uno::digitalWrite(pin.trig, LOW);
        lastCalc = millis();
    }

    if (finishedCalc)
    {
        distance = duration * 0.034 / 2;
        Serial.print("\n\ndistance: ");
        Serial.print(distance);
        Serial.print(" cm \n IR-left: ");
        Serial.print(irStatus[left]);
        Serial.print(" \n IR-right: ");
        Serial.print(irStatus[right]);
        finishedCalc = false;
    }

    while (firstLoop and uno::digitalRead(pin.btn) == HIGH)
    {
        delay(10);
    }
    firstLoop = false;

    // ===========================================================================================================================

    switch (currentState)
    {
    case DRIVING:

        if (distance <= 20) // obstical detected
        {
            // stopVehicle();
            servo.write(180);
            timerLoop = millis();
            currentState = OBSTICAL_DETECTED;
        }
        else if (irStatus[left] == HIGH && irStatus[right] == HIGH)
        {
            robot.drive(FORWARDS)[255];
        }
        else if (irStatus[left] == LOW && irStatus[right] == HIGH)
        {
            robot.drive(RIGHT)[255];
        }
        else if (irStatus[left] == HIGH && irStatus[right] == LOW)
        {
            robot.drive(LEFT)[255];
        }
        else if (irStatus[left] == LOW && irStatus[right] == LOW)
        {
            currentState = LOST_TRACK;
        }
        break;

    case OBSTICAL_DETECTED:
        robot.drive(LEFT)[150];

        if (millis() - timerLoop >= 2000) // ANPASSEN ------------------------------------------------------------------------------
        {
            timerRoute = millis();
            currentState = FINDING_NEW_ROUTE;
        }
        break;

    case FINDING_NEW_ROUTE:
        if (distance <= 25)
        {
            robot.drive(FORWARDS)[255];
        }

        if (distance >= 24)
        {

            robot.drive(RIGHT)[150];

            if (millis() - timerRoute >= 2000) // ANPASSEN ---------------------------------------------------------------------------
            {
                if (distance <= 26)
                {
                    robot.drive(FORWARDS)[255];
                }
                if (distance >= 25)
                {
                    robot.drive(RIGHT)[150];

                    if (digitalRead(pin.irLeft) == HIGH || digitalRead(pin.irRight) == HIGH) // bool maybe <-------
                    {
                        servo.write(90);
                        currentState = DRIVING;
                    }
                }
            }
        }
        break;

    case LOST_TRACK:
        robot.drive(STOP_Y)[__];
        break;
    }
    // ===========================================================================================================================
}

void echoInterrupt()
{
    if (digitalRead(pin.echo) == HIGH)
    {
        startTime = micros();
    }
    else
    {
        duration = micros() - startTime;
        finishedCalc = true;
    }
}

void irStatusSetter()
{
    irStatus[left] = digitalRead(pin.irLeft);
    irStatus[right] = digitalRead(pin.irRight);
}

/*bool pauseFor(uint32_t &previousTimeStamp, uint32_t waitTime)
{
  uint32_t currentTimeStamp = millis();

  if (currentTimeStamp - previousTimeStamp >= waitTime)
  {
    previousTimeStamp = currentTimeStamp;
    return true;
  }

  return false;
}*/
