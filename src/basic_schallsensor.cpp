#include <Arduino.h>
#include <stdint.h>

uint32_t duration;
uint32_t distance;

constexpr uint8_t trigPin = 9;
constexpr uint8_t echoPin = 2; // interrupterpin

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
}

void loop()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.034 / 2;

  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}
