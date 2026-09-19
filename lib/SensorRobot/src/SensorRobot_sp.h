// clang-format off
/*
  SensorRobot_sp.h - Robot libary for Arduino - Version 2.2
  Copyright (c) 2026 Tim Seiffer. All right reserved.
  _________________________________________________________________________
  
  ||  IF YOU GET YOUR HANDS ON THIS COPY DELETE IT YOU SCHOULDNT USE IT  ||
  ||  ONLY IF YOU ALREADY USER 'optmzdArd.h' OTHERWISE THIS LIB WONT     ||
  ||  WORK                                                               ||
  _________________________________________________________________________

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
  A basic Robot it initilized per construktor (4 motors for driving)
  if additional Sensors are wanted, the attach method can be 
  called after everthing is configurated with the methods your Robot should 
  be able to drive, scan distances, detect and drive around object and drive 
  along a line with two or one IR-Senor/s
  --HOW-IT-WORKS--

    SensorRobot objName (leftWheelOfTheBack, rightWheelOfTheBack                 | this is the contructor which
                         leftWheelOfTheFront, rightWheelOfTheFront);             | lets you do the basics like drive
            
    objName.attach( overloaded+3 );                                              | so far you can attach two types of Sensor and a Servo  - This method has overloads  
    objName.attach( servoName );                                                 | one Servo can be attached for an ultra sonic sensor
    objName.attach( ultraSonicTrig, ultraSonicEcho );                            | one Ultra Sonic Sensor with trig and echo can be attached as well
    objName.attach( leftIR, rightIR, lineColor);                                 | and two ir for following the track, the color of the track NEEDS to be given aswell
                
    objName.setPid( kp ,ki, kd );                                                | this method is used for methods like drive.correctError or drive.alongLineTillError
    objName.setBaseSpeed( baseSpeed );                                           | this method is for any non-User related driving if not called baseSpeed ist per default 150 
    objName.setThesholds( blackIrVal, whiteIrVal );                              | NECESSARY METHOD FOR IR's. if you dont know the vals for your irs use method calibrate( pin ) and hold your ir above the a black and white spot
                
    objName.begin();                                                             | just sets all current configirated pins as INPUT or OUTPUT pls only call in setup
    objName.readDistance();                                                      | reads the distance using ultra sonic sensor
    objName.readDistanceTillClear();                                             | lets you scan an obj and return both sides and for every side it gives a bool back if its true and the distance till clear (data stored in irStatus)

    irStaus irVals;                                                              | this is how to unpack the data from 'readDistanceTillClear()'
    if (irVals.rightClear)...;
    if (irVals.rightSide >= val)...;   

    objName.readDistanceTillClear( side );                                       | this method lets you read the distance from obj till clear but only on the prefered side

    objName.calculateError( currentSensorValue );                                | this method is for one ir if you have two this will still work just way worse it returns error there
    objName.calculateError();                                                    | this method is built for Two ir sensors it is a upgraded version of the one ir version it also uses pid and can get errors for drive.correctError

  for any drive related use put an drive. infront of method, exambles:

    objName.drive( overload+3 )[ speed ]                                         | the basic drive method which drive with baseSpeed  - This method has overloads 
    objName.drive( YDir )[ speed ];        ex: robot.drive(FORWARDS)[255];       | this method lets you drive Back- or For-wards (or stop with keyword STOP_Y)
    objName.drive( XDir )[ speed ];        ex: robot.drive(LEFT)[255];           | this method lets you drive to the Right or Left (or stop with keyword STRAIGHT)
    objName.drive( YDir, XDir )[ speed ];  ex: robot.drive(FORWARDS, LEFT)[255]; | this method is the combination of both and used for smoth curves instead of sharp ones 

    objName.drive.direct( speedLeftWheel, speedRightWheel );                     | this method lets YOU decide the speed and how sharp the curves are. to controll the wheels in the back use a negative val (for pid) 
    objName.drive.direct( speedLeftWheel, speedRightWheel, SMOOTH_ACCEL );       | if smooth acceleration is wanted just add an SMOOTH_ACCEL to the two parameters
    objName.drive.aroundObj < (NON-)BLOCKING > ();                               | lets you to drive around an object in BLOCKING or NON_BLOCKING mode 

    objName.drive.correctError( error );                                         | uses PID to stay on the line it works whith the calculateError method and can be uses for singel and multi ir setup
    objName.drive.correctError();                                                | this will automaticly decide based on your setup if it should call correct for one or two Irs

    objName.drive.alongLineTillError < (NON-)BLOCKING > ();                      | this method lets users just skip the hole driving part this method correct automaticly and can be blocking or non-blocking
    objName.drive.alongLineTillError < (NON-)BLOCKING > ( distanceGate );        | same thing as method without distanceGate only acception that it also looks for obj with ultra sonic, distanceGate it the var to determite at what point it should count as a obsticle
*/

#pragma once
#ifndef sensor_robot_sp_h
#define sensor_robot_sp_h

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
namespace gpio = uno;
#else
namespace gpio = ard;
#endif

#include "optmzdArd.h"
#include <stdint.h>
#include <Servo.h>

// namespace sensRbt {
constexpr uint8_t noSpeed       {0};     // #define noSpeed 0

constexpr uint8_t RIGHT_SIDE    {0};     // #define RIGHT_SIDE 0
constexpr uint8_t LEFT_SIDE     {1};     // #define LEFT_SIDE 1

constexpr uint8_t ERROR         {1};     // #define ERROR 1
constexpr uint8_t NO_ERROR      {0};     // #define NO_ERROR 0
constexpr uint8_t IR_LOST_LINE  {2};     // #define IR_LOST_LINE 2
constexpr uint8_t OBJ_DETECTED  {3};     // #define OBJ_DETECTED 3

constexpr uint8_t BLOCKING      {0};     // #define BLOCKING 0
constexpr uint8_t NON_BLOCKING  {1};     // #define NON_BLOCKING 1

constexpr bool BLACK            {true }; // #define BLACK 1
constexpr bool WHITE            {false}; // #define WHITE 0
constexpr uint8_t SMOOTH_ACCEL  {  1  }; // #define enableSmoothAcceleration 1
constexpr bool KD               {false}; // these are new makros
constexpr bool KP               {true }; // 
constexpr bool PID              {false}; // 
constexpr bool IR_VALS          {true }; // 

constexpr float SPEED_OF_SOUND_CM_PER_US {0.0343f};

#ifdef __cplusplus
    constexpr unsigned long long operator"" s  ( long double s         )  { return s * 1000.0; }
    constexpr unsigned long long operator"" ms ( unsigned long long ms )  { return ms;         }
    constexpr unsigned long long operator"" cm ( unsigned long long cm )  { return cm;         }
    constexpr long double        operator"" cm ( long double cm        )  { return cm;         }
    constexpr unsigned long long operator"" mm ( unsigned long long mm )  { return mm / 10;    }
    constexpr long double        operator"" mm ( long double mm        )  { return mm / 10;    }
#endif
// }
enum YDir { // enum class YDir : uint8_t {
    FORWARDS,
    BACKWARDS,
    STOP_Y,
    BACKWARDS_NO_Y
};

enum XDir { // enum class XDir : uint8_t {
    LEFT,
    RIGHT,
    STRAIGHT
};

class SensorRobot
{
    friend class DriveMethodsEx;
    friend class DriveMethodsSe;
    friend class DriveExecutor;
    friend class DriveSelector;

protected:
    struct irStatus {
        float leftSide, rightSide;
        bool leftClear, rightClear;
    };
    uint8_t _rB, // the motor for the Right wheel in the Back
        _lB,     // the motor for the left wheel in the Back
        _rF,     // the motor for the Right wheel in the Front
        _lF;     // the motor for the left wheel in the Front

    int _kp,
        _ki,
        _kd;

    bool _BLACK_LINE : 1;
    bool _WHITE_LINE : 1;
    bool _DEFINED_LINECOL : 1;
    bool _ATTACHED_MULTI_IR : 1;
    bool _ATTACHED_SINGLE_IR : 1;
    bool _ATTACHED_US : 1;
    bool _HAS_SERVO : 1;
    bool _FORTH_WHEELER : 1;

    bool _BLACK_LINE{false}, WHITE_LINE{false}, DEFINED_LINECOL{false}, ATTACHED_MULTI_IR{false};
    bool _ATTACHED_SINGLE_IR{false}, ATTACHED_US{false}, HAS_SERVO{false}, FORTH_WHEELER{false};

    int _blackVal,
        _whiteVal,
        _threshold,
        _linecol;

    int _previousError{0};
    int _integral{0};
    int _baseSpeed{150};

    bool _objIsSquare{false};

    YDir _y;
    XDir _x;

    uint8_t _servoPin, _irLeft, _irRight, _usEcho, _usTrig;

    int _currentLeft{0};
    int _currentRight{0};
    uint32_t _lastRampTime{0};

    Servo *_servoPtr{nullptr};

    int readIrPrecentage(uint8_t pin);
    float readDistance();
    void direct(int speedLeft, int speedRight);
    void correctError(int error);
    void stayOnObjTillChange();
    void stayOnObjTillLineIsFound();

public:
    DriveSelector drive;
    void begin(void);

    explicit SensorRobot(uint8_t pinLeFr, uint8_t pinRiFr, uint8_t pinLeBa, uint8_t pinRiBa);
    explicit SensorRobot(uint8_t pinLeFr, uint8_t pinRiFr);

    void setPid(float kp, float ki, float kd);
    void setBaseSpeed(int speed);
    void attach(Servo &userServo);
    void attach(uint8_t trig, uint8_t echo);
    void attach(uint8_t irPin, bool linecol);
    void attach(uint8_t irLeft, uint8_t irRight, bool linecol);
    void setThresholds(uint8_t whiteVal, uint8_t blackVal);
    // void calibrate(uint8_t pin);
    void calibrate(bool PIDorIR);
    float getPid(bool KPorKD);
    uint8_t getIrValue(bool blackOrWhite);

    uint8_t readIr(uint8_t pin);
    float readDistance(void);
    irStatus readDistanceTillClear(void);
    float readDistanceTillClear(bool side);
    int calculateError(uint16_t pin);
    int calculateError(void);

    bool leftSideIsClear();
    float leftSideObjLenght();
    bool rightSideIsClear();
    float rightSideObjLenght();
};

class DriveExecutor
{
private:
    SensorRobot& _p;

public:
    explicit DriveExecutor(YDir y, XDir x, SensorRobot& parent);
    void operator[](uint8_t speed);
};

// <----------------------------------------------------------------------------------------- class XDirectionSelector and class YDirectionSelector

class DriveSelector
{
private:
    SensorRobot& _p;
    bool robot_to_obj_done{false};       // this->drive.aroundObj ONLY
    bool servo_done_turning{false};      // this->drive.aroundObj ONLY
    bool first_loop_in_servo_turn{true}; // this->drive.aroundObj ONLY
    uint16_t temp{};                     // this->drive.aroundObj ONLY

public:
    explicit DriveSelector(SensorRobot& parent): _p(parent){}
    DriveExecutor operator()(YDir y, XDir x); // this was originaly operator[] but the std for avr chips is not c++24 and only there multible parameter for operator[] was added
    DriveExecutor operator()(XDir x);
    DriveExecutor operator()(YDir y);
    void initMotors(uint8_t pLeFr, uint8_t pRiFr, uint8_t pLeBa, uint8_t pRiBa);
    void initMotors(uint8_t pLeFr, uint8_t pRiFr);

    template<bool MODE = false> void direct(int speedLeft, int speedRight) 
    {
        if (MODE == SMOOTH_ACCEL)
        {
            uint32_t now = uno::millis();

            if (now - _p._lastRampTime >= 10)
            {
                _p._lastRampTime = now;

                int step = 15;

                if (_p._currentLeft < targetLeft)
                {
                    _p._currentLeft += step;
                    if (_p._currentLeft > targetLeft)
                        _p._currentLeft = targetLeft;
                }
                else if (_p._currentLeft > targetLeft)
                {
                    _p._currentLeft -= step;
                    if (_p._currentLeft < targetLeft)
                        _p._currentLeft = targetLeft;
                }
                if (_p._currentRight < targetRight)
                {
                    _p._currentRight += step;
                    if (_p._currentRight > targetRight)
                        _p._currentRight = targetRight;
                }
                else if (_p._currentRight > targetRight)
                {
                    _p._currentRight -= step;
                    if (_p._currentRight < targetRight)
                        _p._currentRight = targetRight;
                }
            }
        
            int leftOut = _p._currentLeft;
            if (leftOut >= 0)
            {
                if (leftOut > 255)
                    leftOut = 255;
                gpio::analogWrite(_p._lF, leftOut);
                gpio::analogWrite(_p._lB, 0);
            }
            else if (_p._FORTH_WHEELER)
            {
                leftOut = -leftOut;
                if (leftOut > 255)
                    leftOut = 255;
                gpio::analogWrite(_p._lF, 0);
                gpio::analogWrite(_p._lB, leftOut);
            }
        
            int rightOut = _p._currentRight;
            if (rightOut >= 0)
            {
                if (rightOut > 255)
                    rightOut = 255;
                gpio::analogWrite(_p._rF, rightOut);
                gpio::analogWrite(_p._rB, 0);
            }
            else if (_p._FORTH_WHEELER)
            {
                rightOut = -rightOut;
                if (rightOut > 255)
                    rightOut = 255;
                gpio::analogWrite(_p._rF, 0);
                gpio::analogWrite(_p._rB, rightOut);
            }
        }
        else {
            _p.direct(speedLeft, speedRight);
        }
    }
    template<bool MODE = false> bool aroundObj(void) 
    {
        
        if (MODE == NON_BLOCKING) // das IST KACKE ich habs mir einfacher vorgestellt aber das ist
        {                         // nen scheiß deswegen erklärung für späteres ich
                                  // als erstes benutzen wir kein 'sicherheit guard' weil zu lansam ist
            int linecol{(_p._linecol == BLACK) ? _p._blackVal : _p._whiteVal};
        
            if (_p.readDistance() > 7cm && !robot_to_obj_done) { // wir fahren so lange gerade bis ditanz 7cm ist
                direct(_p._baseSpeed, _p._baseSpeed);            // !robot_to_obj_done ist weil wir ja sehr oft in diese funtion sein werden und
            }                                                    // weiter unten benutzen wir ne andere methode un da darf es NICHT gerade ausfahren
            if (_p.readDistance() <= 7cm && !servo_done_turning) {
                robot_to_obj_done = true; // robot_to_obj_done auf true das nicht wieder die obere task repeated wird
                direct(0, 0);
                _p._servoPtr->write(180); // servo nach rechts für später

                if (first_loop_in_servo_turn) { // wichtig sonst funktioniert der timer nicht
                    temp = uno::millis();
                    first_loop_in_servo_turn = false;
                }
                if (uno::millis() - temp < 0.2s)   // timer von 200ms wärenddessen fährt auto nach rechts 
                {                             // das ganze hier ist notwendig weil sonst wärend dem turnen des autos
                    direct(0, _p._baseSpeed); // das weiter unten ausgelöst werden kann das istz nicht gut
                    if (_p.readDistance() > 6.9cm)
                        servo_done_turning = true;
                }
            }               
            if (servo_done_turning) {
            
                if (_p.readDistance() < 7cm)
                    direct(0, _p._baseSpeed);
                else
                    direct(_p._baseSpeed, 0);

                if (linecol == _p._blackVal && (_p.readIrPrecentage(_p._irLeft) > 20 || _p.readIrPrecentage(_p._irRight) > 20))
                    servo_done_turning = false;
                    robot_to_obj_done = false;
                    first_loop_in_servo_turn = true;
                    temp = 0;
                    return 1;
                if (linecol == _p._whiteVal && (_p.readIrPrecentage(_p._irLeft) < 80 || _p.readIrPrecentage(_p._irRight) < 80))
                    servo_done_turning = false;
                    robot_to_obj_done = false;
                    first_loop_in_servo_turn = true;
                    temp = 0;
                    return 1;
            }
            return 0;
        }
        else
        {
            if (_p._servoPtr == NULL || !_p._ATTACHED_US)
                return;
            if (!_p._ATTACHED_MULTI_IR || !_p._ATTACHED_SINGLE_IR)
                return;
            int linecol{(_p._linecol == BLACK) ? _p._blackVal : _p._whiteVal};
            // hier diesmal mein denkprozess
            direct(_p._baseSpeed, _p._baseSpeed); // hier lassen wir den roboter so lange fahren bis
            while (_p.readDistance() > 7cm)      // er auf 7cm abstand mit obj ist für später
                ;                                 // null statement

            direct(0, 0);  // stop lass servo erstmal drehen
            _p._servoPtr->write(180);
            uno::delay(200);                      // das hier drunter ist nur so weil ich später in for(;;) eine schlechte 'calc' amch wenn man das so überhaupt nennen kann
            direct(0, _p._baseSpeed);        // jz fahren wir so lange rechts bis distanz wieder 5cm ist weil am anfang ist ja
            while (_p.readDistance() > 7cm) // servo nicht parallel zu obj und sonst der roboter dauerhaft nach links oder im kreis fahren würde
                ;                            // null statement

            for (;;) // null statement?? egal du weiß eh was das ist mach das nur wegen fy dude auf tt
            {
                if (_p.readDistance() < 7cm)
                    direct(0, _p._baseSpeed);
                else
                    direct(_p._baseSpeed, 0);
                if (linecol == _p._blackVal && (_p.readIrPrecentage(_p._irLeft) > 20 || _p.readIrPrecentage(_p._irRight) > 20))
                    break;
                if (linecol == _p._whiteVal && (_p.readIrPrecentage(_p._irLeft) < 80 || _p.readIrPrecentage(_p._irRight) < 80))
                    break;
            }
            uint16_t othermillis{uno::millis()};
            uint16_t now{uno::millis()};
            while (now - othermillis < 1.0s)
                correctError();
            return 1;
        }
    }
    template<bool MODE = false> bool alongLineTillError(float distanceGate = NAN)
    {
        if (!_p._ATTACHED_US || !_p._HAS_SERVO)
            return ERROR;
        if (!_p._ATTACHED_MULTI_IR || !_p._ATTACHED_SINGLE_IR)
            return ERROR;

        if (MODE == NON_BLOCKING)
        {
            int leftVal = _p.readIrPrecentage(_p._irLeft);
            int rightVal = _p.readIrPrecentage(_p._irRight);
            if (!isnan(distanceGate))
            {
                int ultraSonicVal = _p.readDistance();
                if (ultraSonicVal <= distanceGate)
                    return OBJ_DETECTED;
            }

            int lineThresholds = 40;
            bool leftOnLine = (leftVal > lineThresholds);
            bool rightOnLine = (rightVal > lineThresholds);

            if (_p._WHITE_LINE)
            {
                leftOnLine = (leftVal < lineThresholds);
                rightOnLine = (rightVal < lineThresholds);
            }

            if (!leftOnLine && !rightOnLine)
            {
                _p.direct(0, 0);
                return IR_LOST_LINE;
            }
            int lineError = leftVal - rightVal;
            _p.correctError(lineError);

            return NO_ERROR;
        }
        else
        {
            for (;;)
            {
                int leftVal = _p.readIrPrecentage(_p._irLeft);
                int rightVal = _p.readIrPrecentage(_p._irRight);
                if (!isnan(distanceGate))
                {
                    int ultraSonicVal = _p.readDistance();
                    if (ultraSonicVal <= distanceGate)
                        return OBJ_DETECTED;
                }

                int lineThresholds = 40;
                bool leftOnLine = (leftVal > lineThresholds);
                bool rightOnLine = (rightVal > lineThresholds);

                if (_p._WHITE_LINE)
                {
                    leftOnLine = (leftVal < lineThresholds);
                    rightOnLine = (rightVal < lineThresholds);
                }

                if (!leftOnLine && !rightOnLine)
                {
                    _p.direct(0, 0);
                    return IR_LOST_LINE;
                }

                int lineError = leftVal - rightVal;
                _p.correctError(lineError);
                uno::delay(10);

                _p.direct(_p._baseSpeed, _p._baseSpeed);
            }
        }
    }

    void correctError(int error);
    void correctError(void);
};


#endif