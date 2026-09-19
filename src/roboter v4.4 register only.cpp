#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>
#include "optmzdArd.h"
#include "SensorRobot.h"
#include <EEPROM.h>
#include <ArxContainer.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
namespace gpio = uno;
#else
namespace gpio = ard;
#endif // clang-format off

// static constexpr bool nb{1};
#define black 220        // still need calibration
#define acceptbl_black 190 // still needs ajustments
#define white 20         // still need calibration

int error{0};
int lastError{0};
int correction{0};

// default values but do what is descriped below
float kp{0.5}; // still needs to be calibrated (set to 0.1, add small steps like 0.1 -> 0.3 -> 0.5 -> 0.8 ... till he shackes)
float kd{1.2}; // still needs to be calibrated (set to 0, add agein in steps but alittle bit bigger like 0 -> 0.5 -> 1 -> 1.8 ... till he stops shacking)

int baseSpeed{180};
// clang-format off
/* 2,3,__4__,5,__6__,__7__,8,9,10,11,12 */ 
// struct pinConfig { uint16_t servo : 4; uint16_t btn : 4; uint16_t echo : 4; uint16_t trig : 4; /* 1 uint16_t (unsigned int) */
//                    uint16_t riBa : 4; uint16_t leBa : 4; uint16_t riFr : 4; uint16_t leFr : 4; /* 1 uint16_t (unsigned int) */
//                    uint8_t irLeft : 4; uint8_t irRight : 4; }; /* 1 uint8_t (unsigned char) */
// const pinConfig pin{ 11, 12, 2, 9,
//                      3, 8, 5, 10,
//                      A0, A1};

namespace task {
    void switch_if(bool statment);
    void rebuild_before(void);
    volatile bool has_not_switched{true};
    volatile bool has_switched{false};
};
namespace pin {
    constexpr uint8_t servo   {11};
    constexpr uint8_t btn     {12};
    constexpr uint8_t echo     {2};
    constexpr uint8_t trig     {9};
    constexpr uint8_t riBa     {3};
    constexpr uint8_t leBa     {8};
    constexpr uint8_t riFr     {5};
    constexpr uint8_t leFr    {10};
    constexpr uint8_t irLeft  {A0};
    constexpr uint8_t irRight {A1};
    constexpr uint8_t bit_servo   {3};
    constexpr uint8_t bit_btn     {4};
    constexpr uint8_t bit_echo    {2};
    constexpr uint8_t bit_trig    {1};
    constexpr uint8_t bit_riBa    {3};
    constexpr uint8_t bit_leBa    {0};
    constexpr uint8_t bit_riFr    {5};
    constexpr uint8_t bit_leFr    {2};
    constexpr uint8_t bit_irLeft  {0};
    constexpr uint8_t bit_irRight {1};
};
// clang-format on

Servo servo;
SensorRobot robot(pin::leFr, pin::riFr, pin::leBa, pin::riBa); // lf, rf, lb, rb
// std::unique_ptr<SensorRobot> robot = std::make_unique<SensorRobot>(pin.riBa, pin.leBa, pin.riFr, pin.leFr);

volatile uint32_t startTime{0};
volatile uint32_t duration{0};
volatile bool finishedCalc{false};
volatile bool wasInTimerRoute{false};

bool firstLoop = true;
float distance{999};

uint32_t timerLoop{0};
uint32_t timerRoute{0};
// clang-format off
ISR(INT1_vect) { 
    if (PIND & (1 << pin::bit_echo)) {
        startTime = gpio::micros();
    } else {
        duration = gpio::micros() - startTime;
        distance = duration * 0.0343f / 2.0f; // duration / 58.3f
    }
}

enum class STATE : uint8_t{ DRIVING, OBSTICAL_DETECTED, LOST_TRACK };
// clang-format on
STATE current_state = STATE::DRIVING;
std::array<int, 2> irValue = {0, 0};

void setup(void)
{
    TCCR0A = (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);

    TIMSK0 |= (1 << TOIE0);
    // using namespace pin;
    DDRC &= ~(1 << pin::bit_irLeft);
    PORTC &= ~(1 << pin::bit_irLeft);
    DDRC &= ~(1 << pin::bit_irRight);
    PORTC &= ~(1 << pin::bit_irRight);

    DDRB |= (1 << pin::bit_trig);
    PORTB &= ~(1 << pin::bit_trig);
    DDRD &= ~(1 << pin::bit_echo);
    PORTD &= ~(1 << pin::bit_echo);

    DDRB &= ~(1 << pin::bit_btn);
    PORTB |= (1 << pin::bit_btn);

    servo.attach(pin::servo);
    robot.setPid(kp, 0, kd);
    robot.attach(servo);
    robot.attach(static_cast<uint8_t>(pin::trig), static_cast<uint8_t>(pin::echo));
    robot.attach(pin::irLeft, pin::irRight, BLACK);
    robot.setThresholds(white, black);

    Serial.begin(115200);

    EICRA &= ~((1 << ISC10) | (1 << ISC11));
    EICRA |= (1 << ISC10);

    EIMSK |= (1 << INT1);

    servo.write(90);
}

void loop(void)
{
    // using namespace sensRbt;
    irValue[left] = gpio::analogRead<pin::irLeft>();
    irValue[right] = gpio::analogRead<pin::irRight>();

    static uint32_t lastCalc{gpio::millis()};
    if (gpio::millis() - lastCalc >= 0.2s)
    {
        gpio::digitalWrite<pin::trig>(LOW);
        gpio::delayMicroseconds(2);
        gpio::digitalWrite<pin::trig>(HIGH);
        gpio::delayMicroseconds(10);
        gpio::digitalWrite<pin::trig>(LOW);
        lastCalc = gpio::millis();
    }

    while (firstLoop and gpio::digitalRead<pin::btn>())
    {
        gpio::delay(10);
    }
    firstLoop = false;

    // ===========================================================================================================================

    switch (current_state)
    {
    case STATE::DRIVING:

        if (distance <= 8.0cm)
        {
            servo.write(180);
            timerLoop = gpio::millis();
            current_state = STATE::OBSTICAL_DETECTED;
            break;
        }

        irValue[left] = gpio::analogRead<pin::irLeft>();
        irValue[right] = gpio::analogRead<pin::irRight>();

        int error = irValue[left] - irValue[right];

        int pPortion = error * kp;
        int dPortion = (error - lastError) * kd;
        int correction = pPortion + dPortion;

        lastError = error;

        int speedLeft = baseSpeed + correction;
        int speedRight = baseSpeed - correction;

        robot.drive.direct(speedLeft, speedRight, SMOOTH_ACCEL);

        if (irValue[left] < 150 and irValue[right] < 150) // still needs to be calibrated (look what value is white on your irSensor)
        {
            current_state = STATE::LOST_TRACK;
        }
        break;

    case STATE::OBSTICAL_DETECTED:

        task::switch_if(distance <= 12.0cm);
        if (task::has_not_switched)
            robot.drive(FORWARDS)[255];

        if (task::has_switched)
        {
            if (robot.drive.aroundObj<NON_BLOCKING>())
            {
                current_state = STATE::DRIVING;
                task::rebuild_before();
            }
        }

        break;

    case STATE::LOST_TRACK:

        robot.drive(BACKWARDS)[100];
        if (irValue[left] > black and irValue[right] > black)
            current_state = STATE::DRIVING;
        break;
    }
    // ===========================================================================================================================
}
// clang-format off
void task::switch_if(bool switched) {
    if (switched) {
        task::has_not_switched = false;
        task::has_switched = true;
    }
} 
void task::rebuild_before(void) {
    task::has_not_switched = true;
    task::has_switched = false;
}



// bool pausedFor(uint32_t &previousTimeStamp, uint32_t waitTime)
// {
//     uint32_t currentTimeStamp = millis();

//     if (currentTimeStamp - previousTimeStamp >= waitTime)
//     {
//         previousTimeStamp = currentTimeStamp;
//         return true;
//     }

//     return false;
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

// // _____pattern-matching example part1 (setting it up)_____
// // helper type for the visitor
// template<class... Ts>                                       // variatic template
// struct overloaded : Ts... { using Ts::operator()...; };
// // explicit deduction guide (not needed as of C++20)
// template<class... Ts>                                       // variatic template
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
