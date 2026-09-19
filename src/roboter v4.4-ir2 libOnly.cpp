#include <Arduino.h> // clang-format off
#include <Servo.h>
#include "SensorRobot.h"
#define Left A0
#define Right A1

Servo servo;
SensorRobot robot(10, 5, 8, 3); // uint16_t leFrPin{10}; uint16_t riFrPin{5}; uint16_t leBaPin{8}; uint16_t riBaPin{3};
enum class robotState : uint8_t { DRIVING, OBSTICAL_DETECTED, LOST_TRACK};
robotState currentState{robotState::DRIVING};

void setup() {
    servo.attach(11);                                               // uint16_t servoPin{11};
    robot.attach(servo);                                            //
    robot.attach(static_cast<uint8_t>(9), static_cast<uint8_t>(2)); // uint16_t trigPin{9}; uint16_t echoPin{2};
    robot.attach(Left, Right, BLACK);                               // uint8_t irLeftPin  {A0}; uint8_t irRightPin {A1};
    robot.setPid(0.5, 0, 1.2);                                      // float kp{0.5}; float kd{1.2};
    robot.setBaseSpeed(180);                                        // int baseSpeed{180};
    robot.setThresholds(30, 230);                                   // uint8_t white{30}; uint8_t black{230};
    robot.begin();                                                  //
    servo.write(90);                                                //
}

void loop() {
    switch (currentState)
    {
    case robotState::DRIVING:
        if (robot.drive.alongLineTillError<NON_BLOCKING>(10cm) == OBJ_DETECTED) {
            currentState = robotState::OBSTICAL_DETECTED;
            break;
        }
        if (robot.readIr(Left) < 10 && robot.readIr(Right) < 10) {
            currentState = robotState::LOST_TRACK;
        }
        break;

    case robotState::OBSTICAL_DETECTED:
        robot.drive.aroundObj<BLOCKING>();
        currentState = robotState::DRIVING;
        break;

    default:
        break;
    }   
}