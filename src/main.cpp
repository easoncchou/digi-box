#include <Arduino.h>

#include "LiquidCrystal.h"
#include "ezBuzzer.h"

#define JOYSTICK_BUFFER 100UL

// put function declarations here:
int senseDistance();
int detectSound();
typedef struct {
  int x;
  int y;
  int sw;
} joystickData;
joystickData readJoystick();
int handleJoystick4Dir(joystickData);
void appTransitionMachine(int);
void appStateMachine();
void digiPet();

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
unsigned long joystickDelay = JOYSTICK_BUFFER;
// ultrasonic sensor
const int trigPin = 10;
const int echoPin = 9;
// buzzer
const int buzzerPin = 6;
ezBuzzer buzzer(buzzerPin);  // create ezBuzzer object that attach to a pin;
// app FSM
int appState = 0;
/*
0 - main menu
1 - pet-hover
2 - pet
3 - slots-hover
4 - slots
5 - hop-hover
6 - hop
*/
int menuState = 0;
/*

*/
int joystickMelody[] = {NOTE_A5};
int joystickNoteDuration[] = {32};
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
  // Serial.println(joystickDelay);
  joystickData joystick = readJoystick();
  appTransitionMachine(handleJoystick4Dir(joystick));
  appStateMachine();
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



int handleJoystick4Dir(joystickData joystickData) {
  // if there hasn't been enough time between inputs, return
  if (joystickDelay < JOYSTICK_BUFFER) {
    joystickDelay++;
    return 0;
  }
  // returns: 0 -> nothing | 1 -> button pressed | 2 -> up | 3 -> right | 4 ->
  // down | 5 -> left
  if (!joystickData.sw) {
    joystickDelay = 1;
    buzzer.playMelody(joystickMelody, joystickNoteDuration, 1);
    Serial.println(1);
    return 1;
  }
  if (joystickData.y < 20) {
    joystickDelay = 1;
    buzzer.playMelody(joystickMelody, joystickNoteDuration, 1);
    Serial.println(2);
    return 2;
  }
  if (joystickData.x > 1000) {
    joystickDelay = 1;
    buzzer.playMelody(joystickMelody, joystickNoteDuration, 1);
    Serial.println(3);
    return 3;
  }
  if (joystickData.y > 1000) {
    joystickDelay = 1;
    buzzer.playMelody(joystickMelody, joystickNoteDuration, 1);
    Serial.println(4);
    return 4;
  }
  if (joystickData.x < 20) {
    joystickDelay = 1;
    buzzer.playMelody(joystickMelody, joystickNoteDuration, 1);
    Serial.println(5);
    return 5;
  }
  return 0;
}

void appTransitionMachine(int input) { // use an FSM to control the global appState
  switch (appState) {
    case 0: // 0 - main menu
      if (input != 0) // any action
        appState = 1;
      break;
    case 1: // 1 - pet-hover
      if (input == 1) { 
        appState = 2;
      } else if (input == 3) {
        appState = 3;
      } else if (input == 5) {
        appState = 5;
      }
      break;
    case 2: // 2 - pet
      break;
    case 3: // 3 - slots-hover
      if (input == 1) { 
        appState = 4;
      } else if (input == 3) {
        appState = 5;
      } else if (input == 5) {
        appState = 1;
      }
      break;
    case 4: // 4 - slots
      if (input != 0) // any action
        appState = 0;
      break;
    case 5: // 5 - hop-hover
      if (input == 1) { 
        appState = 6;
      } else if (input == 3) {
        appState = 1;
      } else if (input == 5) {
        appState = 3;
      }
      break;
    case 6: // 6 - hop
      if (input != 0) // any action
        appState = 0;
      break;
    default:
      break;
  }
}

void appStateMachine() {
  switch (appState) {
    case 0: // 0 - main menu
      lcd.setCursor(0, 0);
      lcd.print("   Digi-Box!   ");
      lcd.setCursor(0, 1);
      lcd.print("Pet  Slots  Hop!");
      break;
    case 1: // 1 - pet-hover
      lcd.setCursor(0, 1);
      lcd.print("~Pet  Slots  Hop!");
      break;
    case 2: // 2 - pet
      lcd.setCursor(0, 0);
      lcd.print("    Digi-Pet    ");
      lcd.setCursor(0, 1);
      lcd.print("Wave, Chat, Nap!");
      break;
    case 3: // 3 - slots-hover
      lcd.setCursor(0, 1);
      lcd.print("Pet  ~Slots  Hop!");
      break;
    case 4: // 4 - slots
      lcd.setCursor(0, 0);
      lcd.print("  Coming Soon!  ");
      lcd.setCursor(0, 1);
      lcd.print("Press to return.");
      break;
    case 5: // 5 - hop-hover
      lcd.setCursor(0, 1);
      lcd.print("Pet  Slots  ~Hop!");
      break;
    case 6: // 6 - hop
      lcd.setCursor(0, 0);
      lcd.print("  Coming Soon!  ");
      lcd.setCursor(0, 1);
      lcd.print("Press to return.");
      break;
    default:
      break;
  }
}

void digiPet() {
  
}