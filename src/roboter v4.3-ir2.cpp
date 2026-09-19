#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>
#include "optmzdUno.h"
#include "SensorRobot.h"
#include <EEPROM.h>
#include <ArxContainer.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
namespace gpio = uno;
#else
namespace gpio = ard;
#endif

int error{0};
int lastError{0};
int correction{0};

// default values but do what is descriped below
float kp{0.5}; // still needs to be calibrated (set to 0.1, add small steps like 0.1 -> 0.3 -> 0.5 -> 0.8 ... till he shackes)
float kd{1.2}; // still needs to be calibrated (set to 0, add agein in steps but alittle bit bigger like 0 -> 0.5 -> 1 -> 1.8 ... till he stops shacking)

int baseSpeed{180};

// ---------------------------------
struct pinConfig // 2,3,__4__,5,__6__,__7__,8,9,10,11,12
{
    uint16_t servo : 4; // 1 uint16_t (unsigned int)
    uint16_t btn : 4;   //
    uint16_t echo : 4;  //
    uint16_t trig : 4;  //

    uint16_t riBa : 4; // 1 uint16_t (unsigned int)
    uint16_t leBa : 4; //
    uint16_t riFr : 4; //
    uint16_t leFr : 4; //

    uint8_t irLeft : 4;  // 1 uint8_t (unsigned char)
    uint8_t irRight : 4; //
};

const pinConfig pin{
    11, 12, 2, 9,
    3, 8, 5, 10,
    A0, A1};

Servo servo;
SensorRobot robot(pin.leFr, pin.riFr, pin.leBa, pin.riBa); // lf, rf, lb, rb
// std::unique_ptr<SensorRobot> robot = std::make_unique<SensorRobot>(pin.riBa, pin.leBa, pin.riFr, pin.leFr);

volatile uint32_t startTime{0};
volatile uint32_t duration{0};
volatile bool finishedCalc{false};
volatile bool wasInTimerRoute{false};

bool firstLoop = true;
float distance{999};

uint32_t timerLoop{0};
uint32_t timerRoute{0};
// bool pausedFor(uint32_t &previousTimeStamp, uint32_t waitTime);

void echoInterrupt();
// clang-format off
enum class robotState : uint8_t{ DRIVING, OBSTICAL_DETECTED, FINDING_NEW_ROUTE, LOST_TRACK };
// clang-format on
robotState currentState = robotState::DRIVING;
std::array<int, 2> irValue = {0, 0}; // int irValue[2] = {0, 0};
void irStatusSetter();

void setup()
{
    gpio::pinMode(pin.irLeft, INPUT);
    gpio::pinMode(pin.irRight, INPUT);

    gpio::pinMode(pin.trig, OUTPUT);
    gpio::pinMode(pin.echo, INPUT);

    gpio::pinMode(pin.btn, INPUT_PULLUP);
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
    if (millis() - lastCalc >= 0.2s)
    {
        gpio::digitalWrite(pin.trig, LOW);
        delayMicroseconds(2);
        gpio::digitalWrite(pin.trig, HIGH);
        delayMicroseconds(10);
        gpio::digitalWrite(pin.trig, LOW);
        lastCalc = millis();
    }

    if (finishedCalc)
    {
        distance = duration * 0.034f / 2.0f; // duration / 58.0f
        finishedCalc = false;
    }

    while (firstLoop and gpio::digitalRead(pin.btn) == HIGH)
    {
        delay(10);
    }
    firstLoop = false;

    // ===========================================================================================================================

    switch (currentState)
    {
    case robotState::DRIVING:
        if (distance <= 8.0cm)
        {
            servo.write(180);
            timerLoop = millis();
            currentState = robotState::OBSTICAL_DETECTED;
            break;
        }

        irValue[left] = gpio::analogRead(pin.irLeft);
        irValue[right] = gpio::analogRead(pin.irRight);

        int error = irValue[left] - irValue[right];

        int pPortion = error * kp;
        int dPortion = (error - lastError) * kd;
        int correction = pPortion + dPortion;

        lastError = error;

        int speedLeft = baseSpeed + correction;
        int speedRight = baseSpeed - correction;

        robot.drive.direct(speedLeft, speedRight, SMOOTH_ACCEL);

        if (irValue[left] < 150 && irValue[right] < 150) // still needs to be calibrated (look what value is white on your irSensor)
        {
            currentState = robotState::LOST_TRACK;
        }
        break;
    case robotState::OBSTICAL_DETECTED:
        robot.drive(LEFT)[150];

        if (millis() - timerLoop >= 2.0s) // still needs to be calibrated (look how long he needs for an 90° turn)------------------------------------------------------------------------------
        {
            timerRoute = millis();
            currentState = robotState::FINDING_NEW_ROUTE;
        }
        break;

    case robotState::FINDING_NEW_ROUTE:
        if (distance <= 12.0cm)
        {
            robot.drive(FORWARDS)[255];
            delay(200ms);
        }
        if (distance > 12.0cm)
        {
            if (!wasInTimerRoute)
                robot.drive(RIGHT)[150];

            if (millis() - timerRoute >= 2.0s) // still needs to be calibrated (look how long he needs for an 90° turn)---------------------------------------------------------------------------
            {
                if (distance < 8.3cm) // wont happen anyways its just some random val
                {
                    wasInTimerRoute = true;
                    robot.drive(FORWARDS)[255];
                }
                if (distance > 12.0cm)
                {
                    robot.drive(FORWARDS, RIGHT)[150];

                    if (gpio::digitalRead(pin.irLeft) == HIGH || gpio::digitalRead(pin.irRight) == HIGH)
                    {
                        servo.write(90);
                        wasInTimerRoute = false;
                        currentState = robotState::DRIVING;
                        break;
                    }
                }
            }
        }
        break;
    case robotState::LOST_TRACK:
        robot.drive.direct(0, 0);
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
    irValue[left] = gpio::analogRead(pin.irLeft);
    irValue[right] = gpio::analogRead(pin.irRight);

    error = irValue[left] - irValue[right];

    int pPortion = error * kp; // PD-formel
    int dPortion = (error - lastError) * kd;

    correction = pPortion + dPortion;

    lastError = error;
}

// bool pausedFor(uint32_t &previousTimeStamp, uint32_t waitTime)
// {
//   uint32_t currentTimeStamp = millis();

//   if (currentTimeStamp - previousTimeStamp >= waitTime)
//   {
//     previousTimeStamp = currentTimeStamp;
//     return true;
//   }

//   return false;
// }

// // _____template meta-progranning example_____
// template<typename T>
// struct has_nested_value_type <
//     T,
//     typename std::enable_if<
//         !std::is_void<
//             typename std::decay<
//                 decltype(
//                     typename T::value_type{}
//                 )
//             >::type
//         >::value &&
//         std::is_contructible<
//             typename std::add_pointer<
//                 typename std::conditional<
//                     std::is_reference<typename T::value_type>::value,
//                     typename std::remove_reference<typename T::value_type>::type,
//                     typename T::value_type
//                 >::type
//             >::type,
//             std::nullptr_t
//         >::value,
//         void
//     >::type
// > : std::true_type {};

// struct has_nested_value_type <T, typename std::enable_if<!std::is_void<typename std::decay<decltype(typename T::value_type{})>::type>::value && std::is_contructible<typename std::add_pointer<typename std::conditional<std::is_reference<typename T::value_type>::value, typename std::remove_reference<typename T::value_type>::type, typename T::value_type>::type>::type, std::nullptr_t>::value, void>::type> : std::true_type {};

// // _____pattern-matching example part1 (setting it up)_____
// // helper type for the visitor
// template<class... Ts>
// struct overloaded : Ts... { using Ts::operator()...; };
// // explicit deduction guide (not needed as of C++20)
// template<class... Ts>
// overloaded(Ts...) -> overloaded<Ts...>;
// // _____pattern-matching example part2 (making it more like in rust)_____
// template<typename Variant, typename... Handlers>
// auto Match(Variant&& v, Handlers&&... handlers) {
//     return std::visit(
//         base::Overload{ std::forward<Handlers>(handlers)... },
//         std::forward<Variant>(v)
//     );
// }
// // _____pattern-matcging example part3 (actual code)_____
// return Match(shape                             // 'return std::visit( overloaded{' instead of 'return Match( shape'
//     [](const Circle&  c) { return PI * c.r * c.r; },
//     [](const Rect&    r) { return r.w * e.h; },
//     [](const Square&  q) { return q.a * q.a; },
//     [](const Tri&     t) { return 0.5 * t.b * t.h; },
//     [](const Ellipse& e) { return PI * e.a * e.b; }
// );                                                  // '}, shape);' instead of ');'
