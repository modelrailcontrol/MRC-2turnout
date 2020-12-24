// ==================================================================================================
//
//  Modulstyrning MRC 2turnout
//  Copyright (C) 2020  Peter Kindström
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//    
//  Last modified: 2020-11-21
//  Compiled with: LOLIN(WEMOS) D1 R2 & mini
//
//  Uses the following card definitions:
//   esp8266 by ESP8266 Community - v2.7.4 - https://github.com/esp8266/Arduino
//
//  Uses the following libraries:
//   EasyButton by Evert Arias    - v2.0.1 - https://github.com/evert-arias/EasyButton
//   IotWebConf by Balazs Kelemen - v2.3.1 - https://github.com/prampec/IotWebConf
//   PubSubClient by Nick O'Leary - v2.8.0 - https://pubsubclient.knolleary.net/
//
// ==================================================================================================

// --------------------------------------------------------------------------------------------------
//  Includes
// --------------------------------------------------------------------------------------------------

// Include all libraries
#include <PubSubClient.h>     // Handle MQTT subcription & publish
#include <IotWebConf.h>       // Handle wifi connection & client settings
#include <EasyButton.h>       // Handle button presses
#include "mrcServo.h"         // Handle turnout servos
#include "mrcStatus.h"        // Handle status LEDs


// Include settings and functions sketch files
#include "mrcSettings.h"
#include "mrcWifi.h"
#include "mrcMqtt.h"


// --------------------------------------------------------------------------------------------------
//  Create objects
// --------------------------------------------------------------------------------------------------

// Create Servo object and assign pin
mrcServo servoVx1a(pinVx1aServo);
mrcServo servoVx1b(pinVx1bServo);
mrcServo servoVx2a(pinVx2aServo);
mrcServo servoVx2b(pinVx2bServo);


// Create Status LED object and assign pin
mrcStatus ledVx1Rakt(pinVx1Led1);
mrcStatus ledVx1Turn(pinVx1Led2);
mrcStatus ledVx2Rakt(pinVx2Led1);
mrcStatus ledVx2Turn(pinVx2Led2);


// Create Button object and set initial values
EasyButton vx1button(pinVx1Button, 100, false, true);
EasyButton vx2button(pinVx2Button, 100, false, true);


// --------------------------------------------------------------------------------------------------
//  Setting things up
// --------------------------------------------------------------------------------------------------
void setup() { 
  if (debug == 1) {Serial.begin(9600);Serial.println("");}

  // ------------------------------------------------------------------------------------------------
  // Button setup (in the EasyButton.h file)

  // Initialize the button
  vx1button.begin();
  vx2button.begin();

  // Define callback function to be called when a button is pressed
  vx1button.onPressed(btn1Pressed);
  vx2button.onPressed(btn2Pressed);


  // ------------------------------------------------------------------------------------------------
  // IotWebConfig setup

  // Initial setup (in the mrcWifi.h file)
  wifiSetup();

  // Set up required URL handlers for the config web pages
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });


  // -------------------------------------------------------------------------------------------------
  // MQTT setup

  // Define topics (in the mrcMqtt.h file)
  mqttSetup();

  // Wait for IotWebServer to start network connection
  delay(2000);

  // Connect to MQTT broker
  int mqttPort = atoi(cfgMqttPort);
  mqttClient.setServer(cfgMqttServer, mqttPort);

  // Define function to handle callbacks
  mqttClient.setCallback(mqttCallback);

  
  // -----------------------------------------------------------------------------------------------
  // Servo setup (in the mrcWifi.h file)

  // Servo limits
  // xxx.limit(min degree, max degree, time interval in ms)
  servoVx1a.limits(servo1aMin,servo1aMax,servo1aMove,servo1aBack);
  servoVx1b.limits(servo1bMin,servo1bMax,servo1bMove,servo1bBack);
  servoVx2a.limits(servo2aMin,servo2aMax,servo2aMove,servo2aBack);
  servoVx2b.limits(servo2bMin,servo2bMax,servo2bMove,servo2bBack);

  // Define which funktion to call when a servo has stopped moving
  servoVx1a.onFinished(servo1aFinished);
  servoVx1b.onFinished(servo1bFinished);
  servoVx2a.onFinished(servo2aFinished);
  servoVx2b.onFinished(servo2bFinished);

}

// --------------------------------------------------------------------------------------------------
//  Run forever... 
// --------------------------------------------------------------------------------------------------
void loop() { 

  // Check connection to the MQTT broker. If no connection, try to reconnect
  if (needMqttConnect) {
    if (mqttConnect()) {
      needMqttConnect = false;
    }
  } else if ((iotWebConf.getState() == IOTWEBCONF_STATE_ONLINE) && (!mqttClient.connected())) {
    Serial.println("MQTT reconnect");
    Serial.print("Client connected = ");
    Serial.println(mqttClient.connected());
    mqttConnect();
  }

  // Run repetitive jobs
  mqttClient.loop();            // Wait for incoming MQTT messages
  iotWebConf.doLoop();          // Check for IotWebConfig actions
  vx1button.read();             // Check for button pressed
  ledVx1Rakt.loop();            // Check for/perform led action
  ledVx1Turn.loop();            // Check for/perform led action
  servoVx1a.loop();             // Check for/perform servo action
  servoVx1b.loop();             // Check for/perform servo action
  vx2button.read();             // Check for button pressed
  ledVx2Rakt.loop();            // Check for/perform led action
  ledVx2Turn.loop();            // Check for/perform led action
  servoVx2a.loop();             // Check for/perform servo action
  servoVx2b.loop();             // Check for/perform servo action

  // Blink both status leds if we have just started after a power off
  if (hasStarted == 1) {
      if (debug == 1) {Serial.println(dbText+"Start");}
      ledVx1Rakt.blink(1500);     // Set blink rate 1500 ms for status led 1a
      ledVx1Turn.blink(1500);     // Set blink rate 1500 ms for status led 1b
      ledVx2Rakt.blink(1500);     // Set blink rate 1500 ms for status led 2a
      ledVx2Turn.blink(1500);     // Set blink rate 1500 ms for status led 2b
      hasStarted = 0;             // Don't run this routine again (until next power off/on)
  }
    
} 

// --------------------------------------------------------------------------------------------------
//  Function that gets called when button 1 is pressed
// --------------------------------------------------------------------------------------------------
void btn1Pressed () {
    if (debug == 1) {Serial.println(dbText+"Button 1 pressed");}

     // Indicate moving turout
     mqttPublish(pubTurnout1State, "moving", RETAIN);

    // Toggle button function
    if (button1State == 0) {
      if (debug == 1) {Serial.println(dbText+"Turnout 1 set to closed");}
      servoVx1a.closed();
      servoVx1b.closed();
      ledVx1Rakt.blink(500);
      ledVx1Turn.off();
      button1State = 1;
    } else if (button1State == 1) {
      if (debug == 1) {Serial.println(dbText+"Turnout 1 set to thrown");}
      servoVx1a.thrown();
      servoVx1b.thrown();
      ledVx1Rakt.off();
      ledVx1Turn.blink(500);
      button1State = 0;
    }

}

// --------------------------------------------------------------------------------------------------
//  Function that gets called when button 2 is pressed
// --------------------------------------------------------------------------------------------------
void btn2Pressed () {
    if (debug == 1) {Serial.println(dbText+"Button 2 pressed");}

     // Indicate moving turnout
     mqttPublish(pubTurnout1State, "moving", RETAIN);

    // Toggle button function
    if (button2State == 0) {
      if (debug == 1) {Serial.println(dbText+"Turnout 2 set to closed");}
      servoVx2a.closed();
      servoVx2b.closed();
      ledVx2Rakt.blink(500);
      ledVx2Turn.off();
      button2State = 1;
    } else if (button2State == 1) {
      if (debug == 1) {Serial.println(dbText+"Turnout 2 set to thrown");}
      servoVx2a.thrown();
      servoVx2b.thrown();
      ledVx2Rakt.off();
      ledVx2Turn.blink(500);
      button2State = 0;
    }

}

// --------------------------------------------------------------------------------------------------
//  Function that gets called when servo 1a stops moving
// --------------------------------------------------------------------------------------------------
void servo1aFinished () {
  if (debug == 1) {Serial.println(dbText+"Servo 1a finished");}

  // Set LEDs only if both servos are at end position
  if (servoVx1b.status() == 0){

    // Check in which position servo is and set status LEDs accordingly
    if (servoVx1a.position() == 0) {
      ledVx1Rakt.off();
      ledVx1Turn.on();
      mqttPublish(pubTurnout1State, "thrown", RETAIN);
    } else {
      ledVx1Rakt.on();
      ledVx1Turn.off();
      mqttPublish(pubTurnout1State, "closed", RETAIN);
    }

  }
}

// --------------------------------------------------------------------------------------------------
//  Function that gets called when servo 1b stops moving
// --------------------------------------------------------------------------------------------------
void servo1bFinished () {
  if (debug == 1) {Serial.println(dbText+"Servo 1b finished");}

  // Set LEDs only if both servos are at end position
  if (servoVx1a.status() == 0){
    
    // Check in which position servo is and set status LEDs accordingly
    if (servoVx1b.position() == 0) {
      ledVx1Rakt.off();
      ledVx1Turn.on();
      mqttPublish(pubTurnout1State, "thrown", RETAIN);
    } else {
      ledVx1Rakt.on();
      ledVx1Turn.off();
      mqttPublish(pubTurnout1State, "closed", RETAIN);
    }
  }

}

// --------------------------------------------------------------------------------------------------
//  Function that gets called when servo 2a stops moving
// --------------------------------------------------------------------------------------------------
void servo2aFinished () {
  if (debug == 1) {Serial.println(dbText+"Servo 2a finished");}

  // Set LEDs only if both servos are at end position
  if (servoVx2b.status() == 0){

    // Check in which position servo is and set status LEDs accordingly
    if (servoVx2a.position() == 0) {
      ledVx2Rakt.off();
      ledVx2Turn.on();
      mqttPublish(pubTurnout2State, "thrown", RETAIN);
    } else {
      ledVx2Rakt.on();
      ledVx2Turn.off();
      mqttPublish(pubTurnout2State, "closed", RETAIN);
    }
  }

}

// --------------------------------------------------------------------------------------------------
//  Function that gets called when servo 2b stops moving
// --------------------------------------------------------------------------------------------------
void servo2bFinished () {
  if (debug == 1) {Serial.println(dbText+"Servo 2b finished");}

  // Set LEDs only if both servos are at end position
  if (servoVx2a.status() == 0){
    
    // Check in which position servo is and set status LEDs accordingly
    if (servoVx2b.position() == 0) {
      ledVx2Rakt.off();
      ledVx2Turn.on();
      mqttPublish(pubTurnout2State, "thrown", RETAIN);
    } else {
      ledVx2Rakt.on();
      ledVx2Turn.off();
      mqttPublish(pubTurnout2State, "closed", RETAIN);
    }
  }

}

// --------------------------------------------------------------------------------------------------
// Function to handle MQTT messages sent to this device
// --------------------------------------------------------------------------------------------------
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Don't know why this have to be done :-(
  payload[length] = '\0';

  // Make strings
  String msg = String((char*)payload);
  String tpc = String((char*)topic);

  // Print the topic and payload
  if (debug == 1) {Serial.println(dbText+"Recieved: "+tpc+" = "+msg);}

  // Check for Turnout 1 commands
  if (tpc == subTopic[0]) {
    if (hasStarted == 1) {hasStarted = 0; };

    if (msg == "toggle") { btn1Pressed(); }
    else if (msg == "closed") {
      if (servoVx1a.position() == 0) { btn1Pressed(); }
    }
    else if (msg == "thrown") { 
      if (servoVx1a.position() == 1) { btn1Pressed(); }
    }
  }
  
  // Check for Turnout 2 commands
  if (tpc == subTopic[1]) {
    if (hasStarted == 1) {hasStarted = 0; };

    if (msg == "toggle") { btn2Pressed(); }
    else if (msg == "closed") {
      if (servoVx2a.position() == 0) { btn2Pressed(); }
    }
    else if (msg == "thrown") { 
      if (servoVx2a.position() == 1) { btn2Pressed(); }
    }
  }

}

// --------------------------------------------------------------------------------------------------
//  Function that gets called when IotWebConf web page config has been saved
// --------------------------------------------------------------------------------------------------
void configSaved()
{
  if (debug == 1) {Serial.println(dbText+"IotWebConf config saved");}
  deviceID = String(cfgDeviceId);
  deviceName = String(cfgDeviceName);

  // Update settings for turnout 1
  servo1aMin = atoi(cfgservo1aMin);
  servo1aMax = atoi(cfgservo1aMax);
  servo1aMove = atoi(cfgservo1aMove);
  servo1aBack = atoi(cfgservo1aBack);
  servoVx1a.limits(servo1aMin,servo1aMax,servo1aMove,servo1aBack);

  servo1bMin = atoi(cfgServo1bMin);
  servo1bMax = atoi(cfgServo1bMax);
  servo1bMove = atoi(cfgServo1bMove);
  servo1bBack = atoi(cfgServo1bBack);
  servoVx1b.limits(servo1bMin,servo1bMax,servo1bMove,servo1bBack);

  // Update settings for turnout 1
  servo2aMin = atoi(cfgservo2aMin);
  servo2aMax = atoi(cfgservo2aMax);
  servo2aMove = atoi(cfgservo2aMove);
  servo2aBack = atoi(cfgservo2aBack);
  servoVx2a.limits(servo2aMin,servo2aMax,servo2aMove,servo2aBack);

  servo2bMin = atoi(cfgServo2bMin);
  servo2bMax = atoi(cfgServo2bMax);
  servo2bMove = atoi(cfgServo2bMove);
  servo2bBack = atoi(cfgServo2bBack);
  servoVx2b.limits(servo2bMin,servo2bMax,servo2bMove,servo2bBack);

}
