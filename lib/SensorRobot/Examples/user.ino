#include <Arduino.h>
#include <Servo.h>
#include <SensorRobot.h>

byte leftWheelInBack = 2; // NEEDS AN PWM PIN ON YOUR BOARD PWM PINS ARE MARKED WHITH ~
byte rightWheelInBack = 3;
byte leftWheelInFront = 4;
byte rightWheelInFront = 5;

byte servoPin = 6;
byte usTrigPin = 7;
byte usEchoPin = 8;
byte leftIrPin = 9;
byte rightIrPin = 10;

float p = 0.5;
float i = 0;
float d = 1.2;

Servo servo;
SensorRobot robot(leftWheelInBack, rightWheelInBack,
                  leftWheelInFront, rightWheelInFront);

void setup()
{
  servo.attach(servoPin);
  robot.attach(servo);
  robot.attach(usTrigPin, usEchoPin);
  robot.attach(leftIrPin, rightIrPin, BLACK);
  robot.setBaseSpeed(200);
  robot.setThresholds(50, 200);
  robot.setPid(p, i, d);
  robot.begin();
}
void loop()
{
  { // ex 1. very simpel but without any control
    while (robot.drive.alongLineTillError(10cm)[NON_BLOCKING] == !OBJ_DETECTED)
      ;
    robot.drive.aroundObj();
  }
  { // ex 2. advanced with more control
    robot.drive.correctError(robot.calculateError());
    if (robot.readDistance() < 10cm)
      robot.drive.aroundObj();
    if (robot.readIr(leftIrPin) < 20 || robot.readIr(rightIrPin) < 20)
      for (;;)
        robot.drive.direct(0, 0);
  }
  { // ex 2. advanced with more control
    robot.drive.correctError(robot.calculateError());
    if (robot.readDistance() < 10cm)
      robot.drive.aroundObj();
    if (robot.readIr(leftIrPin) < 20 || robot.readIr(rightIrPin) < 20)
      for (;;)
        robot.drive.direct(0, 0);
  }
}