// ==================================================================================================
//
//  MRC Status class
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
#include "mrcStatus.h"

// --------------------------------------------------------------------------------------------------
//  Main class function
// --------------------------------------------------------------------------------------------------
mrcStatus::mrcStatus(byte pin) {
  this->pin = pin;        // Get pin number to use for LED
  init();
}

// --------------------------------------------------------------------------------------------------
//  Initialize the status class
// --------------------------------------------------------------------------------------------------
void mrcStatus::init() {
  pinMode(pin, OUTPUT);   // Set LED output pin
  action = OFF;           // Start with no action/status
}

// --------------------------------------------------------------------------------------------------
//  Take care of repetitive tasks
// --------------------------------------------------------------------------------------------------
void mrcStatus::loop() {
  unsigned long currentMillis = millis();
  switch (action) {

    // Turn LED off
    case OFF:
        digitalWrite(pin, LOW);
      break;

    // Turn LED on
    case ON:
        digitalWrite(pin, HIGH);
      break;

    // Make LED blinking with 'interval' milliseconds interval
    case BLINK:
      if(currentMillis - previousMillis > interval) {

        // Save the last time we blinked the LED 
        previousMillis = currentMillis;   

        // If the LED is off turn it on and vice-versa
        if (state == 1) {
          digitalWrite(pin, HIGH);
          state = 0;
        } else {
          digitalWrite(pin, LOW);
          state = 1;
        }

      }
      break;
  }
}

// --------------------------------------------------------------------------------------------------
//  Turn on LED
// --------------------------------------------------------------------------------------------------
void mrcStatus::on() {
  action = ON;
  if (debug == 1) {Serial.println(dbText+"Led pin"+pin+" ON");}
}

// --------------------------------------------------------------------------------------------------
//  Turn off LED
// --------------------------------------------------------------------------------------------------
void mrcStatus::off() {
  action = OFF;
  if (debug == 1) {Serial.println(dbText+"Led pin"+pin+" OFF");}
}

// --------------------------------------------------------------------------------------------------
//  Make LED blinking
//  time: How long the LED will be turned on/off, in milliseconds
// --------------------------------------------------------------------------------------------------
void mrcStatus::blink(int time) {
  action = BLINK;
  this->interval = time;
  if (debug == 1) {Serial.println(dbText+"Led BLINK");}
}

// --------------------------------------------------------------------------------------------------
//  Get LED status
//  Returns:
//    0 = If 'action' = OFF   i.e LED turned off
//    1 = If 'action' = ON    i.e LED turned on
//    2 = If 'action' = BLINK i.e LED is blinking
// --------------------------------------------------------------------------------------------------
boolean mrcStatus::status() {
  if (debug == 1) {Serial.println(dbText+"Return status = "+action);}
  return action;
}
