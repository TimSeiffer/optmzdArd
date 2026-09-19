#include <Arduino.h>
#include <stdint.h>

constexpr uint8_t trigPin = 9;
constexpr uint8_t echoPin = 2;
constexpr uint8_t btnPin = 12;

volatile int32_t starTime = 0;
volatile int32_t duration = 0;
volatile bool finishedCalc = false;

bool firstLoop = true;
void echoInterrupt();

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(btnPin, INPUT_PULLUP);

  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(echoPin), echoInterrupt, CHANGE);
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
    uint16_t distance = duration * 0.034 / 2;
    Serial.print("\n\ndistance: ");
    Serial.print(distance);
    Serial.print(" cm");
    finishedCalc = false;
  }

  // ---------------------------------------------------------------------------
  while (firstLoop && digitalRead(btnPin) == 1)
  {
  }
  firstLoop = false;
  // ---------------------------------------------------------------------------
}

void echoInterrupt()
{
  if (digitalRead(echoPin) == HIGH)
  {
    starTime = micros(); // Echo startet
  }
  else
  {
    duration = micros() - starTime; // Echo beendet
    finishedCalc = true;
  }
}
