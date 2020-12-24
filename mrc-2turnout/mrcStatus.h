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
#include <Arduino.h>

// --------------------------------------------------------------------------------------------------
//  Ensure "mrcStatus.h" is only included/compiled once
// --------------------------------------------------------------------------------------------------
#ifndef mrcStatus_h
#define mrcStatus_h

// --------------------------------------------------------------------------------------------------
//  Class variables and functions
// --------------------------------------------------------------------------------------------------
class mrcStatus {

  // ------------------------------------------------------------------------------------------------
  // Variables & functions used within the class
  private:
    byte debug = 0;                 // Set debug mode (0=off, 1=on)
    String dbText = "Status : ";
    byte pin;
    unsigned long previousMillis;
    int interval = 1000;            // Default blinking interval in milliseconds
    int state = 1;                  // LED state when blinking (0=off, 1=on)
    enum Action {                   // LED action; on, off or blinking
      OFF = 0,
      ON = 1,
      BLINK = 2
    } action;
    
  // ------------------------------------------------------------------------------------------------
  // Variables & functions accessible from outside the class
  public:
    mrcStatus(byte pin);
    void init();
    void loop();
    void on();
    void off();
    void blink(int time);
    int status();
};

#endif
