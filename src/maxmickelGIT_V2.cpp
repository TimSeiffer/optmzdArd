#include <Servo.h>
#include <Arduino.h>

const int TRIG_PIN = 4;
const int ECHO_PIN = 2;

const int IR_PIN_Links = A1;
const int IR_PIN_Rechts = A2;

// motor pins
const int i1 = 5;
const int i2 = 6;
const int i3 = 9;
const int i4 = 10;

Servo servo;
const int ServoPin = 3;

long duration;
int distance;

void AutoVor();

void AutoStop();

void AutoRueckwaerts();

void AutoRechts();

void AutoLinks();

int Scan();

void setup()
{
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(i1, OUTPUT);
  pinMode(i2, OUTPUT);
  pinMode(i3, OUTPUT);
  pinMode(i4, OUTPUT);

  pinMode(IR_PIN_Links, INPUT);
  pinMode(IR_PIN_Rechts, INPUT);

  Serial.begin(9600); // Zum checken von Ultraschal und IR

  servo.attach(ServoPin);
}

void loop()
{
  const int IRValueLinks = digitalRead(IR_PIN_Links); // falls du das nicht mehr änderst bnutze const int
  const int IRValueRechts = digitalRead(IR_PIN_Rechts);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2; // hast btw die formel falscxh gehabt du hattest statt '0.034' '0.038'

  Serial.print("\n\n\ndistance: "); // einfach nur zum testen
  Serial.print(distance);
  Serial.print(" cm\nIR-status: ");
  Serial.print(IRValueLinks);

  if (distance > 30 && IRValueLinks == LOW && IRValueRechts == LOW) // falls es nicht funktioniert mach mal aus 'HIGH' einfach '1' und aus 'LOW' '0'
  {
    AutoVor();
  }
  else if (distance > 30 && IRValueLinks == HIGH && IRValueRechts == LOW)
  {
    AutoLinks();
  }
  else if (distance > 30 && IRValueLinks == LOW && IRValueRechts == HIGH)
  {
    AutoRechts();
  }
  else if (distance > 30 && IRValueLinks == HIGH && IRValueRechts == HIGH)
  {
    AutoStop();
  }

  if (distance < 30)
  {
    AutoStop();
    Scan();
  }
}

void AutoVor()
{
  analogWrite(i1, 0);
  analogWrite(i2, 100);
  analogWrite(i3, 0);
  analogWrite(i4, 100);
}

void AutoStop()
{
  analogWrite(i1, 0);
  analogWrite(i2, 0);
  analogWrite(i3, 0);
  analogWrite(i4, 0);
}

void AutoRueckwaerts()
{
  analogWrite(i1, 100);
  analogWrite(i2, 0);
  analogWrite(i3, 100);
  analogWrite(i4, 0);
}

void AutoRechts()
{
  analogWrite(i1, 0);
  analogWrite(i2, 100);
  analogWrite(i3, 100);
  analogWrite(i4, 0);
}

void AutoLinks()
{
  analogWrite(i1, 100);
  analogWrite(i2, 0);
  analogWrite(i3, 0);
  analogWrite(i4, 100);
}

int Scan()
{
  for (int servoDeg = 90; servoDeg <= 180; servoDeg += 5)
  {
    servo.write(servoDeg);
    delay(100);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;

    if (distance >= 35)
    {
      return 0;
    }
  }

  servo.write(90);
  delay(1000);

  for (int servoDeg = 90; servoDeg <= 0; servoDeg -= 5)
  {
    servo.write(servoDeg);
    delay(100);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;

    if (distance >= 35)
    {
      return 1;
    }
  }

  servo.write(90);
  delay(1000);

  return 3;
}

/* #include <Arduino.h>
#include <Servo.h>

const int TRIG_PIN = 4;
const int ECHO_PIN = 2;

const int IR_PIN_Links = A1;
const int IR_PIN_Rechts = A2;

// motor pins (für mich das ich besser check (dachte am anfang das sind LED's))
const int i1 = 5;
const int i2 = 6;
const int i3 = 9;
const int i4 = 10;

long duration;
int distance;

Servo servo;
const int SERVO_PIN = 3;

void AutoVor();

void AutoStop();

void AutoRueckwaerts();

void AutoRechts();

void AutoLinks();

void scan(String direction);

void setup()
{
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(i1, OUTPUT);
  pinMode(i2, OUTPUT);
  pinMode(i3, OUTPUT);
  pinMode(i4, OUTPUT);

  pinMode(IR_PIN_Links, INPUT);
  pinMode(IR_PIN_Rechts, INPUT);

  servo.attach(SERVO_PIN);

  Serial.begin(9600);
}

void loop()
{
  int IRValueLinks = digitalRead(IR_PIN_Links);
  int IRValueRechts = digitalRead(IR_PIN_Rechts);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2; // hast btw die formel falscxh gehabt du hattest statt '0.034' '0.038'

  Serial.print("\n\n\ndistance: "); // einfach nur zum testen
  Serial.print(distance);           //
  Serial.print(" cm\nIR-status: "); //
  Serial.print(IRValueLinks);       //

  // bei deinem code achtetr er garnicht darauf ob er überhaupt noch hauf der linie ist
  if (distance > 30 && IRValueLinks == LOW && IRValueRechts == LOW) // falls es nicht funktioniert mach mal aus 'HIGH' einfach '1' und aus 'LOW' '0'
  {
    // weg frei dings fährt los
    AutoVor();
  }
  else if (distance > 30 && IRValueLinks == HIGH && IRValueRechts == LOW)
  {
    AutoLinks();
  }
  else if (distance > 30 && IRValueLinks == LOW && IRValueRechts == HIGH)
  {
    AutoRechts();
  }
  else if (distance > 30 && IRValueLinks == HIGH && IRValueRechts == HIGH)
  {
    AutoStop();
  }

  if (distance < 30)
  {
    scan("links");
    AutoRechts();

    unsigned long timeStemp = millis();
    while (millis() - timeStemp < 2000) // oder delay(2000); ist das selbe bloß bissle schlechter
    {                                   // weil dieser code nicht blockiert, delay schon
    }

    scan("rechts");
    AutoRechts();

    unsigned long timeStemp = millis();
    while (millis() - timeStemp < 2000) // oder delay(2000); ist das selbe bloß bissle schlechter
    {                                   // weil dieser code nicht blockiert, delay schon
    }

    while (digitalRead(IR_PIN_Links) == LOW || digitalRead(IR_PIN_Rechts) == LOW)
    {
      AutoVor();
    }
    AutoStop();
  }
}

void AutoVor()
{
  analogWrite(i1, 0);
  analogWrite(i2, 100);
  analogWrite(i3, 0);
  analogWrite(i4, 100);
}

void AutoStop()
{
  analogWrite(i1, 0);
  analogWrite(i2, 0);
  analogWrite(i3, 0);
  analogWrite(i4, 0);
}

void AutoRueckwaerts()
{
  analogWrite(i1, 100);
  analogWrite(i2, 0);
  analogWrite(i3, 100);
  analogWrite(i4, 0);
}

void AutoRechts()
{
  analogWrite(i1, 0);
  analogWrite(i2, 100);
  analogWrite(i3, 100);
  analogWrite(i4, 0);
}

void AutoLinks()
{
  analogWrite(i1, 100);
  analogWrite(i2, 0);
  analogWrite(i3, 0);
  analogWrite(i4, 100);
}

void scan(String direction)
{
  int IRValueLinks = digitalRead(IR_PIN_Links);
  int IRValueRechts = digitalRead(IR_PIN_Rechts);
  int servoDeg;

  (direction == "links") ? servoDeg = 180 : servoDeg = 180;

  servo.write(servoDeg);
  (direction == "links") ? AutoLinks() : AutoRechts();

  unsigned long timeStemp = millis();

  while (millis() - timeStemp < 2000) // oder delay(2000); ist das selbe bloß bissle schlechter
  {                                   // weil dieser code nicht blockiert, delay schon
  }

  while (distance < 30)
  { // clang-format off
    AutoVor();                          // bin mir nd sicher ob das maybe stehen bleibt
    digitalWrite(TRIG_PIN, HIGH);       // weil pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
    delayMicroseconds(10);              // ist auch 'blocking' ich persönlioch hab mein eigenen
    digitalWrite(TRIG_PIN, LOW);        // pulse in programmiert ohne 'blocking' aber das war sehr
    duration = pulseIn(ECHO_PIN, HIGH); // schwer falls aber dieser code nicht funktioniert
    distance = duration * 0.034 / 2;    // dann schreib mich drauf an und ich probier mein bestes es zu fixen
  } // clang-format on

  AutoStop();
} */
