#include <Arduino.h>
#include <stdint.h>
#include <EEPROM.h>
#include <Servo.h>

Servo servo;
constexpr uint8_t irPin = A0;

constexpr uint8_t echoPin = 2;
constexpr uint8_t trigPin = 9;

constexpr uint8_t servoPin = 11;
constexpr uint8_t btnPin = 12;

constexpr uint8_t backLeft = 5;
constexpr uint8_t frontLeft = 6;
constexpr uint8_t backRight = 7;
constexpr uint8_t frontRight = 8;

volatile int32_t startTime = 0;
volatile int32_t duration = 0;
volatile bool finishedCalc = false;

bool firstLoop = true;
uint16_t distance;

uint32_t previousTimeStamp = 0;
bool pauseFor(uint32_t waitTime);
void echoInterrupt();
uint8_t irStatus;
void irStatusSetter();

void driveForwards(uint8_t speed);
void driveBackwards(uint8_t speed);
void turnRight(uint8_t speed);
void turnLeft(uint8_t speed);
void stopVeicle();

bool checkPath(String direction, uint8_t speed);
void findNewRoute();

void setup()
{
  pinMode(irPin, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(btnPin, INPUT_PULLUP);
  servo.attach(servoPin);

  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(echoPin), echoInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(irPin), irStatusSetter, CHANGE);

  servo.write(90);
}

void loop()
{
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
    Serial.print(" cm");
    finishedCalc = false;
  }

  // clang-format off
  // -------------------------------------------//
  while (firstLoop && digitalRead(btnPin) == 1) //
  {                                             //
  }                                             //
  firstLoop = false;                            //
  // -------------------------------------------//-----------------------------------
  // clang-format on

  if (distance > 30 && irStatus == HIGH) // alles fine aka fahren
  {
  driveForwards:
    driveForwards(300);
  }
  else if (irStatus == LOW) // ir findet weg nicht aka abiegen protokol
  {
    stopVeicle();

    if (checkPath("left", 200))
    {
      goto driveForwards;
    }
    if (checkPath("right", 200))
    {
      goto driveForwards;
    }
    else
    {
      Serial.print("ERROR");
    }
  }
  else if (distance <= 5 && irStatus == HIGH) // objekt nah aber linie existiert noch
  {
    goto driveForwards;
  }
  else if (distance < 30 || irStatus == LOW)
  {
    findNewRoute();
  }
}
// ----------------------------------------------------------------------------------

void echoInterrupt()
{
  if (digitalRead(echoPin) == HIGH)
  {
    startTime = micros(); // Echo startet
  }
  else
  {
    duration = micros() - startTime; // Echo beendet
    finishedCalc = true;
  }
}

void irStatusSetter()
{
  if (digitalRead(irPin) == HIGH)
    irStatus = HIGH;
  if (digitalRead(irPin) == LOW)
    irStatus = LOW;
}

bool pauseFor(uint32_t waitTime)
{
  uint32_t currentTimeStamp = millis();

  if (currentTimeStamp - previousTimeStamp < waitTime)
  {
    previousTimeStamp = currentTimeStamp;
    return true;
  }

  return false;
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

void stopVeicle()
{
  analogWrite(backLeft, 0);
  analogWrite(frontLeft, 0);
  analogWrite(backRight, 0);
  analogWrite(frontRight, 0);
}

bool checkPath(String direction, uint8_t speed)
{
  int8_t target = (direction == "right") ? 25 : 155;
  int8_t steps = (direction == "right") ? -5 : 5;

  for (int8_t i = 90; (direction == "right") ? i >= target : i <= target; i += steps)
  {
    servo.write(i);
    pauseFor(50);

    if (irStatus == HIGH)
    {
      servo.write(90);
      pauseFor(300);
      while (irStatus == LOW)
      {
        (direction == "right") ? turnRight(speed) : turnLeft(speed);
      }
      return true;
    }
  }

  return false;
}

void findNewRoute()
{
}