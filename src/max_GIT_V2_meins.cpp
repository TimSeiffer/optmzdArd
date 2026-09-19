#include <Arduino.h> // das ist nur für platformio auf vscode

/*pass auf max du hast paar sachen falsch gemacht und zwar nummer:

    1. probier IMMER mit const zu arbeiten das sagt dem arduino einfach das
       man KEINE änderungen an der variable fornehmen kann also 'const int TRIG_PIN = 4;'
       bleibt auch IMMER ' 4 '

       ( aber nicht wirklich ein fehler einfach eine schlechte gewohnheit
       und kleiner tipp zbsp in Python oder auch in cpp ist es so das wenn du deine Variable in
       all caps schreibst das es ein indikator ist das diese variable NICHT verändert werden darf
       weil python nun mal kein 'const' hat)

    2. dein IR system mit anlogRead ist echt viel zu kopmpieziert benutz einfach digital
       (keine sorge selbst wenn du einen anlogen pin benutzt kannst du ihn trozdem einfach als digital bnutzen)

       (WICHTIG: ich bin davon ausgegangen das die linie weiß ist auf schwarzen boden falls es anderrum ist tausche
       einfach 'HIGH' mit 'LOW' aus)
*/

const int TRIG_PIN = 4;
const int ECHO_PIN = 2;

const int IR_PIN = A1;

// motor pins (für mich das ich besser check (dachte am anfang das sind LED's)) ABER BENENNE DEINE VARIABLEN BESSER
const int i1 = 5;
const int i2 = 6;
const int i3 = 9;
const int i4 = 10;

long duration;
long distance;

void correctCourse(); /* funktionen deklariert man in der regen in cpp erst am ende des codes
                        um struktur zu geben und kleiner tipp probier beim benennen von funktionen
                        immer mit einem verb anzufangen ;)*/
void setup()
{
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(i1, OUTPUT);
  pinMode(i2, OUTPUT);
  pinMode(i3, OUTPUT);
  pinMode(i4, OUTPUT);

  pinMode(IR_PIN, INPUT);

  Serial.begin(9600);
}

void loop()
{
  /*const*/ int IRValue = digitalRead(IR_PIN); // falls du das nicht mehr änderst bnutze const int

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2; // hast btw die formel falscxh gehabt du hattest statt '0.034' '0.038'

  Serial.print("\n\n\ndistance: "); // einfach nur zum testen
  Serial.print(distance);           //
  Serial.print(" cm\nIR-status: "); //
  Serial.print(IRValue);            //

  // bei deinem code achtetr er garnicht darauf ob er überhaupt noch hauf der linie ist
  if (distance > 30 && IRValue == HIGH) // falls es nicht funktioniert mach mal aus 'HIGH' einfach '1' und aus 'LOW' '0'
  {
    // weg frei dings fährt los
    analogWrite(i1, 0);
    analogWrite(i2, 100);
    analogWrite(i3, 0);
    analogWrite(i4, 100);
  }
  else
  {
    analogWrite(i1, 0);
    analogWrite(i2, 0);
    analogWrite(i3, 0);
    analogWrite(i4, 0);
  }

  delay(50); // bau ab und zu kleine pausen ein um dem arduino bissle schnaufpausen zu geben ;)
}

void correctCourse()
{
}
