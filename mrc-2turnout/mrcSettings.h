/*
 * MRC Settings
 */

// -------------------------------------------------------------------------------------------------
// Variable declaration

// Pin
int pinVx1Led1 = D4;          // Which pin the first status led is attached to
int pinVx1Led2 = D3;          // Which pin the second status led is attached to
int pinVx1Button = D2;        // Which pin the first button is attached to
int pinVx1aServo = D1;         // Which pin the first servo is attached to
int pinVx1bServo = RX;         // Which pin the second servo is attached to

// Button
int hasStarted = 1;           // Indicates if we have just started from a power down
int button1State = 1;         // Indicates if the first button has been pressed
//int button2State = 1;         // Indicates if the second button has been pressed

// Debug
byte debug = 0;               // Set to "1" for debug messages in Serial monitor (9600 baud)
String dbText = "Main   : ";  // Debug text

// Device
String deviceID;
String deviceName;

// Servo 1
int servo1aMin;
int servo1aMax;
int servo1aMove;
int servo1aBack;

// Servo 2
int servo1bMin;
int servo1bMax;
int servo1bMove;
int servo1bBack;
