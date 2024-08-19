#include <Arduino.h>

#include "LiquidCrystal.h"
#include "ezBuzzer.h"

// put function declarations here:
int senseDistance();
int detectSound();
typedef struct {
  int x;
  int y;
  int sw;
} joystickData;
joystickData readJoystick();
void playSound(joystickData);
void updateMenu(joystickData);
int handleJoystick4Dir(joystickData);

// LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// sound sensor
const int micPin = A5;
// joystick
const int VRx = A0;
const int VRy = A1;
const int SW = 13;
int lastSWstate = HIGH;
int lastVRx = 0;
int lastVRy = 0;
int prevJoystickSignalMillis = 0;
const int timeBetweenInputsMillis = 500;
// ultrasonic sensor
const int trigPin = 10;
const int echoPin = 9;
// buzzer
const int buzzerPin = 6;
ezBuzzer buzzer(buzzerPin);  // create ezBuzzer object that attach to a pin;
// app FSM
int appState = 0;
/*

*/
int menuState = 0;
/*

*/
int joystickMelody[] = {NOTE_A5};
int joystickNoteDuration[] = {16};
int startupMelody[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
int startupNoteDuration[] = {8, 8, 8, 4};

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("   Digi-Box!   ");
  // Begin serial comms
  Serial.begin(9600);
  // sound sensor aka mic
  pinMode(micPin, INPUT);
  // joystick
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);
  // ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // play startup tune and wait
  buzzer.playMelody(startupMelody, startupNoteDuration, 4);
  delay(2000);
}

void loop() {
  buzzer.loop();
  // set the cursor to column 0, line 1
  // detectSound();
  // senseDistance();
  joystickData joystick = readJoystick();
  playSound(joystick);
}

// put function definitions here:

int senseDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  int duration = pulseIn(echoPin, HIGH);
  int distance = (duration * .0343) / 2;
  // Serial.print("Distance: ");
  // Serial.println(distance);
  return distance;
}

int detectSound() {
  int reading = analogRead(micPin);
  Serial.println(reading);
  return reading;
}

joystickData readJoystick() {
  int x = analogRead(VRx);
  int y = analogRead(VRy);
  int sw = digitalRead(SW);
  // Serial.println("x: " + String(x));
  // Serial.println("y: " + String(y));
  // Serial.println("sw: " + String(sw));
  return {x, y, sw};
}

void playSound(joystickData joystickData) {
  int x = abs(joystickData.x - 512);  // displacement scalar
  int y = abs(joystickData.y - 512);  // displacement scalar
  if (lastSWstate == HIGH && joystickData.sw == LOW) {
    buzzer.playMelody(joystickMelody, joystickNoteDuration, 1);
  } else if (lastVRx < 400 && x > 400) {
    buzzer.playMelody(joystickMelody, joystickNoteDuration, 1);
  } else if (lastVRy < 400 && y > 400) {
    buzzer.playMelody(joystickMelody, joystickNoteDuration, 1);
  }
  lastSWstate = joystickData.sw;
  lastVRx = x;
  lastVRy = y;
}

int handleJoystick4Dir(joystickData joystickData) {
  // if there hasn't been enough time between inputs, return
  unsigned long currentMillis = millis();
  if (currentMillis - prevJoystickSignalMillis < timeBetweenInputsMillis)
    return 0;
  // returns: 0 -> nothing | 1 -> button pressed | 2 -> up | 3 -> right | 4 ->
  // down | 5 -> left
  if (!joystickData.sw) {
    prevJoystickSignalMillis = currentMillis;
    return 1;
  }
  if (joystickData.y < 20) {
    prevJoystickSignalMillis = currentMillis;
    return 2;
  }
  if (joystickData.x > 1000) {
    prevJoystickSignalMillis = currentMillis;
    return 3;
  }
  if (joystickData.y > 1000) {
    prevJoystickSignalMillis = currentMillis;
    return 4;
  }
  if (joystickData.x < 20) {
    prevJoystickSignalMillis = currentMillis;
    return 5;
  }
  return 0;
}

void updateMenu(joystickData joystickData) {}