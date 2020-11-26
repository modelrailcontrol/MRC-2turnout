#include "Arduino.h"
#include "mrcServo.h"

mrcServo::mrcServo(byte pin) {
  this->pin = pin;
  init();
}

// Test med callback
void mrcServo::onFinished(mrcServo::callback_t callback)
{
  onFinished_callback = callback;
}

void mrcServo::init() {
  myservo.attach(pin);        // Attach the servo on pin "pin" to the servo object
  servoAction = NON;
}

void mrcServo::limits(int min, int max, int interval, int backStep) {
  this->minPosition = min-1;
  this->maxPosition = max+1;
  this->moveInterval = interval;
  this->backStep = backStep;
  currentPosition = minPosition;
  if (debug == 1) {Serial.println(dbText+"Set backStep = "+backStep);}
}

void mrcServo::loop(){
  unsigned long currentMillis = millis();
  switch (servoAction) {

    // Servo is idle
    case NON:
        // No action
      break;

    // Servo is moving clockwise
    case MIN:
      if(currentMillis - previousMillis > moveInterval) {
        previousMillis = currentMillis;            // Save the last time we blinked the LED 
        currentPosition--;                         // Step down servo position by 1

        // Check if we reached servo end position
        if (currentPosition <= endPosition) {
          servoAction = NON;
          servoStatus = 0;
          currentPosition = minPosition;
          myservo.write(currentPosition);          // Tell servo to go to start position
          if (debug == 1) {Serial.println(dbText+"Slutposition = "+currentPosition);}
          // Klart! Anropa den funktion som definierats i huvudprogrammet med funktionen "mrcServo.onFinished(...)"
          onFinished_callback();
        } else {
          myservo.write(currentPosition);          // Tell servo to go to start position
          if (debug == 1) {Serial.println(dbText+"Position (min) = "+currentPosition);}
        }
      }
      break;

    // Servo is moving counterclockwise
    case MAX:
      if(currentMillis - previousMillis > moveInterval) {
        // Save the last time we blinked the LED 
        previousMillis = currentMillis;   
        currentPosition++;

        if (currentPosition >= endPosition) {
          servoAction = NON;
          servoStatus = 1;
          currentPosition = maxPosition;
          myservo.write(currentPosition);          // Tell servo to go to start position
          if (debug == 1) {Serial.println(dbText+"Slutposition = "+currentPosition);}
          // Klart! Anropa den funktion som definierats i huvudprogrammet med funktionen "mrcServo.onFinished(...)"
          onFinished_callback();
        } else {
          myservo.write(currentPosition);  // Tell servo to go to start position
          if (debug == 1) {Serial.println(dbText+"Position (max) = "+currentPosition);}
        }
      }
      break;
  }
}

void mrcServo::closed(){
  // Move servo if it is NOT in closed position
  if (endPosition != maxPosition+backStep) {
    servoAction = MAX;
    endPosition = maxPosition+backStep;
    if (debug == 1) {Serial.println(dbText+"Max position = "+maxPosition);}
    if (debug == 1) {Serial.println(dbText+"Moving to = "+endPosition);}
    if (debug == 1) {Serial.println(dbText+"Backstep = "+backStep);}
  }
}
    
void mrcServo::thrown(){
  // Move servo only if it is NOT in thrown position
  if (endPosition != minPosition-backStep) {
    servoAction = MIN;
    endPosition = minPosition-backStep;
    if (debug == 1) {Serial.println(dbText+"Max position = "+minPosition);}
    if (debug == 1) {Serial.println(dbText+"Moving to = "+endPosition);}
    if (debug == 1) {Serial.println(dbText+"Backstep = "+backStep);}
  }
}

// Tell servo to go directly to "newPosition"
void mrcServo::moveTo (int newPosition) {
  myservo.write(newPosition);    
  currentPosition = newPosition;
}

boolean mrcServo::status() {
  return servoAction;
}

boolean mrcServo::position() {
  return servoStatus;
}
    
