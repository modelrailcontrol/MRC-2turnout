/*
 * MRC Settings
 */

// -------------------------------------------------------------------------------------------------
// Variable declaration

// Pin for turnout 1
int pinVx1Led1 = D4;          // Which pin the first status led is attached to
int pinVx1Led2 = D3;          // Which pin the second status led is attached to
int pinVx1Button = D2;        // Which pin the first button is attached to
int pinVx1aServo = D1;        // Which pin the first servo is attached to
int pinVx1bServo = RX;        // Which pin the second servo is attached to

// Pin for turnout 2
int pinVx2Led1 = D0;          // Which pin the first status led is attached to
int pinVx2Led2 = D5;          // Which pin the second status led is attached to
int pinVx2Button = D6;        // Which pin the first button is attached to
int pinVx2aServo = D7;        // Which pin the first servo is attached to
int pinVx2bServo = TX;        // Which pin the second servo is attached to

// Button
int hasStarted = 1;           // Indicates if we have just started from a power down
int button1State = 1;         // Indicates if the first button has been pressed
int button2State = 1;         // Indicates if the second button has been pressed

// Debug
byte debug = 0;               // Set to "1" for debug messages in Serial monitor (9600 baud)
String dbText = "Main   : ";  // Debug text

// Device
String deviceID;
String deviceName;
