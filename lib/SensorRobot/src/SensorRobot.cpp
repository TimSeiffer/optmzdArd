#include <Arduino.h>
#include <Servo.h>
#include "SensorRobot.h"

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
namespace gpio = nuno_sp;
#else
namespace gpio = ardu;
#endif
// clang-format off
bool SensorRobot::leftSideIsClear() {
    irStatus irVal;
    return irVal.leftClear;
}
float SensorRobot::leftSideObjLenght() {
    irStatus irVal;
    return irVal.leftSide;
}
bool SensorRobot::rightSideIsClear() {
    irStatus irVal;
    return irVal.rightClear;
}
float SensorRobot::rightSideObjLenght() {
    irStatus irVal;
    return irVal.rightSide;
}

int SensorRobot::readIrPrecentage(uint8_t pin)
{
    if (pin == NULL)
        return -999;

    int raw = gpio::analogRead(pin);
    int percentage = map(raw, _whiteVal, _blackVal, 0, 100);

    return constrain(percentage, 0, 100);
}
float SensorRobot::readDistance() // clang-format on
{
    if (!_ATTACHED_US)
        return -999.0f;
    gpio::digitalWrite(_usTrig, LOW);
    delayMicroseconds(2);

    gpio::digitalWrite(_usTrig, HIGH);
    delayMicroseconds(10);
    gpio::digitalWrite(_usTrig, LOW);

    volatile uint32_t duration = pulseIn(_usEcho, HIGH);

    return duration * 0.034f / 2.0f;
}
void SensorRobot::direct(int speedLeft, int speedRight)
{
    if (speedLeft >= 0)
    {
        if (speedLeft > 255)
            speedLeft = 255;
        gpio::analogWrite(_lF, speedLeft);
        gpio::analogWrite(_lB, 0);
    }
    else if (_FORTH_WHEELER)
    {
        speedLeft = -speedLeft;
        if (speedLeft > 255)
            speedLeft = 255;
        gpio::analogWrite(_lF, 0);
        gpio::analogWrite(_lB, speedLeft);
    }
    if (speedRight >= 0)
    {
        if (speedRight > 255)
            speedRight = 255;
        gpio::analogWrite(_rF, speedRight);
        gpio::analogWrite(_rB, 0);
    }
    else if (_FORTH_WHEELER)
    {
        speedRight = -speedRight;
        if (speedRight > 255)
            speedRight = 255;
        gpio::analogWrite(_rF, 0);
        gpio::analogWrite(_rB, speedRight);
    }
}
void SensorRobot::correctError(int error)
{
    float p = error;

    _integral += error;

    if (_integral > 255)
        _integral = 255;
    if (_integral < -255)
        _integral = -255;

    float d = error - _previousError;
    _previousError = error;

    float correction = (_kp * p) + (_ki * _integral) + (_kd * d);

    int leftSpeed = _baseSpeed + correction;
    int rightSpeed = _baseSpeed - correction;

    direct(leftSpeed, rightSpeed);
}
void SensorRobot::stayOnObjTillChange()
{
    // using namespace sensRbt;

    if (!_HAS_SERVO || !_ATTACHED_US)
        return;
    if (!_ATTACHED_MULTI_IR || !_ATTACHED_SINGLE_IR)
        return;
    int linecol{(_linecol == BLACK) ? _blackVal : _whiteVal};

    _servoPtr->write(180);
    direct(_baseSpeed, 0);
    delay(200);
    float distanceToObj{readDistance()};
    for (;;)
    {
        if (distanceToObj > 5.0cm)
            direct(_baseSpeed, 0);
        else
            direct(0, _baseSpeed);
    }
}
void SensorRobot::stayOnObjTillLineIsFound()
{
    // using namespace sensRbt;

    if (!_HAS_SERVO || !_ATTACHED_US)
        return;
    if (!_ATTACHED_MULTI_IR || !_ATTACHED_SINGLE_IR)
        return;
    int linecol{(_linecol == BLACK) ? _blackVal : _whiteVal};

    _servoPtr->write(180);
    direct(_baseSpeed, 0);
    delay(200);
    float distanceToObj{readDistance()};
    for (;;)
    {
        if (distanceToObj > 5.0cm)
            direct(_baseSpeed, 0);
        else
            direct(0, _baseSpeed);

        if (linecol == _blackVal && (readIrPrecentage(_irLeft) > 20 || readIrPrecentage(_irRight) > 20))
            break;

        if (linecol == _whiteVal && (readIrPrecentage(_irLeft) < 80 || readIrPrecentage(_irRight) < 80))
            break;

        if (distanceToObj > 25.0cm)
        {
            if (linecol == _blackVal)
                while (readIrPrecentage(_irLeft) < 20 || readIrPrecentage(_irRight) < 20)
                {
                    direct(_baseSpeed, (_baseSpeed / 2));
                }
            else
                while (readIrPrecentage(_irLeft) > 80 || readIrPrecentage(_irRight) > 80)
                {
                    direct(_baseSpeed, (_baseSpeed / 2));
                }
            return;
        }
    }
}

DriveExecutor::DriveExecutor(YDir y, XDir x, SensorRobot &parent) : _p(parent)
{
    _p._y = y;
    _p._x = x;
}
void DriveExecutor::operator[](uint8_t speed)
{
    if (_p._y == STOP_Y)
    {
        if (_p._x == STRAIGHT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, 0);
            gpio::analogWrite(_p._lF, 0);
        }
        else if (_p._x == RIGHT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, 0);
            gpio::analogWrite(_p._lF, speed);
        }
        else if (_p._x == LEFT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, speed);
            gpio::analogWrite(_p._lF, 0);
        }
    }
    else if (_p._y == FORWARDS)
    {
        if (_p._x == STRAIGHT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, speed);
            gpio::analogWrite(_p._lF, speed);
        }
        else if (_p._x == RIGHT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, speed / 2);
            gpio::analogWrite(_p._lF, speed);
        }
        else if (_p._x == LEFT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, speed);
            gpio::analogWrite(_p._lF, speed / 2);
        }
    }
    else if (_p._y == BACKWARDS && _p._FORTH_WHEELER)
    {
        if (_p._x == STRAIGHT)
        {
            gpio::analogWrite(_p._rB, speed);
            gpio::analogWrite(_p._lB, speed);
            gpio::analogWrite(_p._rF, 0);
            gpio::analogWrite(_p._lF, 0);
        }
        else if (_p._x == RIGHT)
        {
            gpio::analogWrite(_p._rB, speed / 2);
            gpio::analogWrite(_p._lB, speed);
            gpio::analogWrite(_p._rF, 0);
            gpio::analogWrite(_p._lF, 0);
        }
        else if (_p._x == LEFT)
        {
            gpio::analogWrite(_p._rB, speed);
            gpio::analogWrite(_p._lB, speed / 2);
            gpio::analogWrite(_p._rF, 0);
            gpio::analogWrite(_p._lF, 0);
        }
    }
    else if (_p._y == BACKWARDS_NO_Y && _p._FORTH_WHEELER)
    {
        if (_p._x == STRAIGHT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, 0);
            gpio::analogWrite(_p._lF, 0);
        }
        else if (_p._x == RIGHT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, speed);
            gpio::analogWrite(_p._rF, 0);
            gpio::analogWrite(_p._lF, 0);
        }
        else if (_p._x == LEFT)
        {
            gpio::analogWrite(_p._rB, speed);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, 0);
            gpio::analogWrite(_p._lF, 0);
        }
    }
    else
    {
        if (_p._x == STRAIGHT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, speed);
            gpio::analogWrite(_p._lF, speed);
        }
        else if (_p._x == RIGHT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, speed / 2);
            gpio::analogWrite(_p._lF, speed);
        }
        else if (_p._x == LEFT)
        {
            gpio::analogWrite(_p._rB, 0);
            gpio::analogWrite(_p._lB, 0);
            gpio::analogWrite(_p._rF, speed);
            gpio::analogWrite(_p._lF, speed / 2);
        }
    }
}

DriveExecutor DriveSelector::operator()(YDir y, XDir x)
{
    return DriveExecutor(y, x, _p);
}
DriveExecutor DriveSelector::operator()(XDir x)
{
    return DriveExecutor(STOP_Y, x, _p);
}
DriveExecutor DriveSelector::operator()(YDir y)
{
    return DriveExecutor(y, STRAIGHT, _p);
}
void DriveSelector::initMotors(uint8_t pLeFr, uint8_t pRiFr, uint8_t pLeBa, uint8_t pRiBa)
{
    _p._FORTH_WHEELER = true;
    _p._lF = pLeFr;
    _p._rF = pRiFr;
    _p._lB = pLeBa;
    _p._rB = pRiBa;
}
void DriveSelector::initMotors(uint8_t pLeFr, uint8_t pRiFr)
{
    _p._FORTH_WHEELER = false;
    _p._lF = pLeFr;
    _p._rF = pRiFr;
    _p._lB = 0;
    _p._rB = 0;
}
void DriveSelector::correctError(int error)
{
    _p.correctError(error);
}
void DriveSelector::correctError(void)
{
    int currentSensorValue{_p.readIrPrecentage(_p._irRight)};
    if (_p._ATTACHED_MULTI_IR)
        _p.correctError(_p.readIrPrecentage(_p._irRight) - _p.readIrPrecentage(_p._irLeft));
    else
        _p.correctError(static_cast<int>(_p._threshold) - static_cast<int>(currentSensorValue));
}

void SensorRobot::begin(void) // clang-format off
{
    if (_FORTH_WHEELER) {
        gpio::pinMode(_rB, OUTPUT);
        gpio::pinMode(_lB, OUTPUT);
        gpio::pinMode(_rF, OUTPUT);
        gpio::pinMode(_lF, OUTPUT);
    }
    else {
        gpio::pinMode(_lF, OUTPUT);
        gpio::pinMode(_rF, OUTPUT);
    }
    if (_ATTACHED_MULTI_IR) {
        gpio::pinMode(_irLeft, INPUT);
        gpio::pinMode(_irRight, INPUT);
    }
    if (_ATTACHED_US) {
        gpio::pinMode(_usTrig, OUTPUT);
        gpio::pinMode(_usEcho, INPUT);
    }
    if (_ATTACHED_SINGLE_IR)
        gpio::pinMode(_irLeft, INPUT);
} // clang-format on

SensorRobot::SensorRobot(uint8_t pinLeFr, uint8_t pinRiFr, uint8_t pinLeBa, uint8_t pinRiBa) : drive(*this)
{
    drive.initMotors(pinLeFr, pinRiFr, pinLeBa, pinRiBa);
}
SensorRobot::SensorRobot(uint8_t pinLeFr, uint8_t pinRiFr) : drive(*this)
{
    drive.initMotors(pinLeFr, pinRiFr);
}

void SensorRobot::setPid(float kp, float ki, float kd)
{
    _kp = kp;
    _ki = ki;
    _kd = kd;
}
void SensorRobot::setBaseSpeed(int speed)
{
    _baseSpeed = speed;
}

void SensorRobot::attach(Servo &userServo)
{
    _servoPtr = &userServo;
    _HAS_SERVO = true;
} // this->servoPtr
void SensorRobot::attach(uint8_t trig, uint8_t echo)
{
    _ATTACHED_US = true;
    _usTrig = trig;
    _usEcho = echo;
}
void SensorRobot::attach(uint8_t irPin, bool linecol)
{
    // using namespace sensRbt;

    if (_ATTACHED_MULTI_IR)
        return;
    _ATTACHED_MULTI_IR = false;
    _ATTACHED_SINGLE_IR = true;
    _irLeft = irPin;
    _irRight = NULL;

    _BLACK_LINE = false;
    _WHITE_LINE = false;

    if (linecol == BLACK)
    {
        _linecol = _blackVal;
        _BLACK_LINE = true;
    }

    if (linecol == WHITE)
    {
        _linecol = _whiteVal;
        _WHITE_LINE = true;
    }
}
void SensorRobot::attach(uint8_t irLeft, uint8_t irRight, bool linecol)
{
    // using namespace sensRbt;

    _ATTACHED_MULTI_IR = true;
    _ATTACHED_SINGLE_IR = false;
    _irLeft = irLeft;
    _irRight = irRight;

    _BLACK_LINE = false;
    _WHITE_LINE = false;

    if (linecol == BLACK)
    {
        _linecol = _blackVal;
        _BLACK_LINE = true;
    }

    if (linecol == WHITE)
    {
        _linecol = _whiteVal;
        _WHITE_LINE = true;
    }
}

void SensorRobot::setThresholds(uint8_t whiteVal, uint8_t blackVal)
{
    _blackVal = blackVal;
    _whiteVal = whiteVal;

    if (_BLACK_LINE)
        _linecol = _blackVal;
    if (_WHITE_LINE)
        _linecol = _whiteVal;

    _threshold = (whiteVal + blackVal) / 2;
}
void SensorRobot::calibrate(bool PIDorIR)
{ // clang-format off
    if (PIDorIR == PID)
    {
        bool kpIsCalibrated{false};
        _kd = 0.1;
        _kp = 0;
        for (;;)
        {
            static uint32_t prev{millis()};
            if (millis() - prev < 0.2s) {
                if (!kpIsCalibrated)
                    _kp += 0.1;
                if (kpIsCalibrated)
                    _kd += 0.5;
                prev = millis();
            }
            if (this->calculateError() < 5 && !kpIsCalibrated) 
                kpIsCalibrated = true;
            else if (this->calculateError() < 5 && kpIsCalibrated) 
                return;
            else 
                this->drive.correctError();
            
        }
    }
    else
    {
        int minVal{1023};
        int maxVal{0};

        unsigned long startTime{millis()};
        unsigned long now{millis()};
        while (now - startTime < 3.0s)
        {
            int current = gpio::analogRead(_irLeft);
            if (current < minVal)
                minVal = current;
            if (current > maxVal)
                maxVal = current;
            if (_ATTACHED_MULTI_IR)
            {
                current = gpio::analogRead(_irRight);
                if (current < minVal)
                    minVal = current;
                if (current > maxVal)
                    maxVal = current;
            }
        }

        _whiteVal = minVal;
        _blackVal = maxVal;
        _threshold = (minVal + maxVal) / 2;
    }
}
float SensorRobot::getPid(bool KPorKD) { // using namespace sensRbt;
    return (KPorKD == KD) ? _kp : _kd;
}
uint8_t SensorRobot::getIrValue(bool blackOrWhite) {    // using namespace sensRbt;
    return (blackOrWhite == BLACK) ? _blackVal : _whiteVal;
}
uint8_t SensorRobot::readIr(uint8_t pin) {
    return readIrPrecentage(pin);
}
SensorRobot::irStatus SensorRobot::readDistanceTillClear(void)
{ // clang-format on
    direct(0, 0);
    float distanceGate{readDistance()};
    irStatus irVals;
    irVals.leftClear = false;
    irVals.rightClear = false;
    irVals.leftSide = 0.0;
    irVals.rightSide = 0.0;

    if (_servoPtr == NULL)
        return;

    float al, bl, cl;
    float ar, br, cr;
    bl = 255;
    br = 255;
    _servoPtr->write(90);

    delay(200);

    al = readDistance();
    ar = al;

    distanceGate += 5;

    for (int8_t i{90}; i >= 0; i--)
    {
        _servoPtr->write(i);
        delay(50);
        if (readDistance() >= distanceGate)
        {
            _servoPtr->write((i + 2));
            delay(50);
            cl = readDistance();
            _servoPtr->write(90);

            delay(200);

            if (cl >= al)
                bl = sqrt((cl * cl) - (al * al));
            else
                bl = 0;

            irVals.leftClear = true;
            irVals.leftSide = bl;
            break;
        }
    }
    if (!irVals.leftClear)
    {
        irVals.leftSide = 255;
    }

    for (uint8_t i{90}; i <= 180; i++)
    {
        _servoPtr->write(i);
        delay(50);
        if (readDistance() >= distanceGate)
        {
            _servoPtr->write((i - 2));
            delay(50);
            cr = readDistance();
            _servoPtr->write(90);

            delay(200);

            if (cr >= ar)
                br = sqrt((cr * cr) - (ar * ar));
            else
                br = 0;

            irVals.rightClear = true;
            irVals.rightSide = br;
            break;
        }
    }
    if (!irVals.rightClear)
    {
        irVals.rightSide = 255;
    }

    return irVals;
}
float SensorRobot::readDistanceTillClear(bool side)
{
    // using namespace sensRbt;

    direct(0, 0);
    float distanceGate{readDistance()};
    if (_servoPtr == NULL)
        return 0.0;

    float a, /*b,*/ c;
    // b = 255;
    _servoPtr->write(90);

    delay(200);

    a = readDistance();

    distanceGate += 5;
    // a^2 + b^2 = c^2 | - a || b^2 = c^2 - a^2
    //
    if (side == LEFT_SIDE) // a^2 + b^2 = c^2 triangle
    {
        for (int8_t i{90}; i >= 0; i--)
        {
            _servoPtr->write(i);
            delay(50);
            if (readDistance() >= distanceGate)
            {
                _servoPtr->write((i + 2));
                delay(50);
                c = readDistance();
                _servoPtr->write(90);

                delay(200);

                return (c >= a) ? sqrt((c * c) - (a * a)) : 0; // b
            }
        }
    }
    if (side == RIGHT_SIDE)
    {
        for (uint8_t i{90}; i <= 180; i++)
        {
            _servoPtr->write(i);
            delay(50);
            if (readDistance() >= distanceGate)
            {
                _servoPtr->write((i - 2));
                delay(50);
                c = readDistance();
                _servoPtr->write(90);

                delay(200);

                return (c >= a) ? sqrt((c * c) - (a * a)) : 0; // b
            }
        }
    }
} // clang-format off
int SensorRobot::calculateError(uint16_t currentSensorValue) { 
    return static_cast<int>(_threshold) - static_cast<int>(currentSensorValue);
}
int SensorRobot::calculateError() {
    return readIrPrecentage(_irRight) - readIrPrecentage(_irLeft);
} // clang-format on
// c!zfscPnF;G:$+G9

/*
  last edited: 05.August.2026
  by Tim Seiffer

  current fix-list:
    void stayOnObjTillChange(void)
    void stayOnObjTillLineIsFound(void)
    void this->drive.aroundObj(uint8_t objLenght)

  current implementation-list:
    void SensorRobot::calibrate(bool PIDorIR)

  potentialimplementation:
    a method that lets you calibrate how mauch time it takes for an 90° turn
*/