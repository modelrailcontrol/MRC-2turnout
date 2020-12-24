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
#include <Arduino.h>
#include <Servo.h> 

// --------------------------------------------------------------------------------------------------
//  Ensure "mrcServo.h" is only included/compiled once
// --------------------------------------------------------------------------------------------------
#ifndef mrcServo_h
#define mrcServo_h


// --------------------------------------------------------------------------------------------------
//  Class variables and functions
// --------------------------------------------------------------------------------------------------
class mrcServo {

  // ------------------------------------------------------------------------------------------------
  // Variables & functions used within the class
  private:
    byte debug = 0;                 // Set debug mode (0=off, 1=on)
    String dbText = "Servo : ";
    byte pin;
    int currentPosition;            // Servo current position
    int maxPosition = 85;           // Servo max position
    int minPosition = 75;           // Servo min position
    int endPosition;                // Detect servo end position
    int moveInterval = 100;         // Time between servo moving one step
    int backStep;                   // Steps to move beyond endpoint and back to endpoint again
    int servoStatus;                // Which endpoint servo is at (0=MIN, 1=MAX)
    unsigned long previousMillis;
    enum Action {                   // Servo action; not moving, moving to MIN or moving to MAX
      NON = 0,
      MIN = 1,
      MAX = 2
    } servoAction;

  // ------------------------------------------------------------------------------------------------
  // Variables & functions accessible from outside the class
  public:
    mrcServo(byte pin);
    Servo myservo;
    typedef void(*callback_t)();                // For Callback function
    void onFinished(callback_t callback);       // For Callback function
    callback_t onFinished_callback;             // For Callback function
    void init();
    void limits(int min, int max, int interval, int backStep);
    void loop();
    void closed();
    void thrown();
    void moveTo (int newPosition);
    int status();
    boolean position();
};

#endif
