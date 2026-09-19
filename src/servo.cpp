#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>
#include <stdint.h>

Servo servo;

constexpr uint8_t servoPin = 11;

void setup()
{
  servo.attach(servoPin);
  Serial.begin(115200);
}

void loop()
{
  servo.write(45);
  delay(400);
  Serial.println("left");

  servo.write(90);
  delay(400);
  Serial.println("middle");

  servo.write(135);
  delay(400);
  Serial.println("right");
}