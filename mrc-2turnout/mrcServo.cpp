// ==================================================================================================
//
//  MRC Servo class
//  Copyright (C) 2020  Peter Kindström
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Last modified: 2020-12-24
//
// ==================================================================================================
#include "Arduino.h"
#include "mrcServo.h"

// --------------------------------------------------------------------------------------------------
//  Main class function
// --------------------------------------------------------------------------------------------------
mrcServo::mrcServo(byte pin) {
  this->pin = pin;
  init();
}

// --------------------------------------------------------------------------------------------------
//  Define a callback function to be called when servo reach endpoints
// --------------------------------------------------------------------------------------------------
void mrcServo::onFinished(mrcServo::callback_t callback)
{
  onFinished_callback = callback;
}

// --------------------------------------------------------------------------------------------------
//  Initialize the status class
// --------------------------------------------------------------------------------------------------
void mrcServo::init() {
  myservo.attach(pin);        // Attach the servo on pin "pin" to the servo object
  servoAction = NON;          // Start with servo stopped
}

// --------------------------------------------------------------------------------------------------
//  [Function]
// --------------------------------------------------------------------------------------------------
void mrcServo::limits(int min, int max, int interval, int backStep) {
  this->minPosition = min-1;
  this->maxPosition = max+1;
  this->moveInterval = interval;
  this->backStep = backStep;
  currentPosition = minPosition;
  if (debug == 1) {Serial.println(dbText+"Set backStep = "+backStep);}
}

// --------------------------------------------------------------------------------------------------
//  Take care of repetitive tasks
// --------------------------------------------------------------------------------------------------
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
        previousMillis = currentMillis;            // Save the last time we moved the servo
        currentPosition--;                         // Step down servo position by 1

        // Check if we reached servo end position
        if (currentPosition <= endPosition) {
          servoAction = NON;
          servoStatus = 0;
          currentPosition = minPosition;
          myservo.write(currentPosition);          // Tell servo to move
          if (debug == 1) {Serial.println(dbText+"Endpoit = "+currentPosition);}
          onFinished_callback();                   // Finished moving. Go to callback function.
        } else {
          myservo.write(currentPosition);          // Tell servo to move
          if (debug == 1) {Serial.println(dbText+"Position (min) = "+currentPosition);}
        }
      }
      break;

    // Servo is moving counterclockwise
    case MAX:
      if(currentMillis - previousMillis > moveInterval) {
        previousMillis = currentMillis;           // Save the last time we moved the servo
        currentPosition++;                        // Step up servo position by 1

        // Check if we reached servo end position
        if (currentPosition >= endPosition) {
          servoAction = NON;
          servoStatus = 1;
          currentPosition = maxPosition;
          myservo.write(currentPosition);          // Tell servo to move
          if (debug == 1) {Serial.println(dbText+"Slutposition = "+currentPosition);}
          onFinished_callback();                   // Finished moving. Go to callback function.
        } else {
          myservo.write(currentPosition);         // Tell servo to move
          if (debug == 1) {Serial.println(dbText+"Position (max) = "+currentPosition);}
        }
      }
      break;
  }
}

// --------------------------------------------------------------------------------------------------
//  Move servo if it is NOT in closed position
// --------------------------------------------------------------------------------------------------
void mrcServo::closed(){
  if (endPosition != maxPosition+backStep) {
    servoAction = MAX;
    endPosition = maxPosition+backStep;
    if (debug == 1) {Serial.println(dbText+"Max position = "+maxPosition);}
    if (debug == 1) {Serial.println(dbText+"Moving to = "+endPosition);}
    if (debug == 1) {Serial.println(dbText+"Backstep = "+backStep);}
  }
}
    
// --------------------------------------------------------------------------------------------------
//  Move servo if it is NOT in thrown position
// --------------------------------------------------------------------------------------------------
void mrcServo::thrown(){
  if (endPosition != minPosition-backStep) {
    servoAction = MIN;
    endPosition = minPosition-backStep;
    if (debug == 1) {Serial.println(dbText+"Max position = "+minPosition);}
    if (debug == 1) {Serial.println(dbText+"Moving to = "+endPosition);}
    if (debug == 1) {Serial.println(dbText+"Backstep = "+backStep);}
  }
}

// --------------------------------------------------------------------------------------------------
// Tell servo to go directly to "newPosition"
// --------------------------------------------------------------------------------------------------
void mrcServo::moveTo (int newPosition) {
  myservo.write(newPosition);    
  currentPosition = newPosition;
}

// --------------------------------------------------------------------------------------------------
//  Get servo status
//  Returns:
//    0 = servo isn't doing anything (NON)
//    1 = servo moving to MIN endpoint (MIN)
//    2 = servo moving to MAX endpoint (MAX)
// --------------------------------------------------------------------------------------------------
boolean mrcServo::status() {
  return servoAction;
}

// --------------------------------------------------------------------------------------------------
// Get servo endpoint
//  Returns:
//    0 = servo at MIN endpoint
//    1 = servo at MAX enpoint
// --------------------------------------------------------------------------------------------------
boolean mrcServo::position() {
  return servoStatus;
}
    
