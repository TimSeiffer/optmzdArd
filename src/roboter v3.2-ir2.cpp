// #include "optmzdUno.h"
#include <SensorRobotV2.h> // gabs damals nicht geadded um rot weggehen zu lassen
#include <Arduino.h>

#include <EEPROM.h>
#include <Servo.h>
#include <stdint.h>

namespace uno = nuno_sp;

constexpr uint8_t left{1};  // #define left 1
constexpr uint8_t right{0}; // #define right 0

Servo servo;
constexpr uint8_t irPinLeft{A0};
constexpr uint8_t irPinRight{A1};

constexpr uint8_t echoPin{2};
constexpr uint8_t trigPin{9};

constexpr uint8_t servoPin{11};
constexpr uint8_t btnPin{12};

constexpr uint8_t frontRight{5};
constexpr uint8_t frontLeft{6};
constexpr uint8_t backRight{7};
constexpr uint8_t backLeft{8};

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

void driveForwards(uint8_t speed);
void driveBackwards(uint8_t speed);
void turnRight(uint8_t speed);
void turnLeft(uint8_t speed);
void stopVehicle();

void setup()
{
  pinMode(irPinLeft, INPUT);
  pinMode(irPinRight, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(btnPin, INPUT_PULLUP);
  servo.attach(servoPin);

  pinMode(backLeft, OUTPUT);
  pinMode(frontLeft, OUTPUT);
  pinMode(backRight, OUTPUT);
  pinMode(frontRight, OUTPUT);

  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(echoPin), echoInterrupt, CHANGE);

  servo.write(90);
}

void loop()
{
  irStatusSetter();

  static uint32_t lastCalc = 0;
  if (millis() - lastCalc >= 500)
  {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
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

  while (firstLoop && digitalRead(btnPin) == HIGH)
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
      driveForwards(255);
    }
    else if (irStatus[left] == LOW && irStatus[right] == HIGH)
    {
      turnRight(255);
    }
    else if (irStatus[left] == HIGH && irStatus[right] == LOW)
    {
      turnLeft(255);
    }
    else if (irStatus[left] == LOW && irStatus[right] == LOW)
    {
      currentState = LOST_TRACK;
    }
    break;

  case OBSTICAL_DETECTED:
    turnLeft(150);

    if (millis() - timerLoop >= 2000) // ANPASSEN ------------------------------------------------------------------------------
    {
      timerRoute = millis();
      currentState = FINDING_NEW_ROUTE;
    }
    break;

  case FINDING_NEW_ROUTE:
    if (distance <= 25)
    {
      driveForwards(255);
    }

    if (distance >= 24)
    {

      turnRight(150);

      if (millis() - timerRoute >= 2000) // ANPASSEN ---------------------------------------------------------------------------
      {
        if (distance <= 26)
        {
          driveForwards(255);
        }
        if (distance >= 25)
        {
          turnRight(150);

          if (digitalRead(irPinLeft) == HIGH || digitalRead(irPinRight) == HIGH) // bool maybe <-------
          {
            servo.write(90);
            currentState = DRIVING;
          }
        }
      }
    }
    break;

  case LOST_TRACK:
    stopVehicle();
    break;
  }
  // ===========================================================================================================================
}

void echoInterrupt()
{
  if (digitalRead(echoPin) == HIGH)
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
  irStatus[left] = digitalRead(irPinLeft);
  irStatus[right] = digitalRead(irPinRight);
}

void driveForwards(uint8_t speed)
{
  analogWrite(backLeft, 0);
  analogWrite(frontLeft, speed);
  analogWrite(backRight, 0);
  analogWrite(frontRight, speed);
}

void driveBackwards(uint8_t speed)
{
  analogWrite(backLeft, speed);
  analogWrite(frontLeft, 0);
  analogWrite(backRight, speed);
  analogWrite(frontRight, 0);
}

void turnRight(uint8_t speed)
{
  analogWrite(backLeft, 0);
  analogWrite(frontLeft, speed);
  analogWrite(backRight, 0);
  analogWrite(frontRight, 0);
}

void turnLeft(uint8_t speed)
{
  analogWrite(backLeft, 0);
  analogWrite(frontLeft, 0);
  analogWrite(backRight, 0);
  analogWrite(frontRight, speed);
}

void stopVehicle()
{
  analogWrite(backLeft, 0);
  analogWrite(frontLeft, 0);
  analogWrite(backRight, 0);
  analogWrite(frontRight, 0);
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
}


void findNewRoute()
{
  if (distance <= 25)
  {
    driveForwards(255);
  }

  if (distance >= 24)
  {
    turnRight(150);
    if (pauseFor(timerRoute, 2000))
    {
      if (distance <= 35)
      {
        driveForwards(255);
      }
      if (distance >= 35)
      {
        turnRight(150);

        while (digitalRead(irPinLeft) == LOW || digitalRead(irPinRight) == LOW)
        {
          delay(10);
        }
      }
    }
  }
}*/