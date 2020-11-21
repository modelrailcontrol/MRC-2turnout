/*
  MRC Wifi settings
*/

// --------------------------------------------------------------------------------------------------
// IotWebConf variables

// Servo 1a
int servo1aMin;
int servo1aMax;
int servo1aMove;
int servo1aBack;

// Servo 1b
int servo1bMin;
int servo1bMax;
int servo1bMove;
int servo1bBack;

// Servo 2a
int servo2aMin;
int servo2aMax;
int servo2aMove;
int servo2aBack;

// Servo 2b
int servo2bMin;
int servo2bMax;
int servo2bMove;
int servo2bBack;

// Default string and number lenght
#define STRING_LEN 32
#define NUMBER_LEN 8

// Access point configuration
const char thingName[] = "MRC-client";                  // Initial AP name, used as SSID of its own Access Point
const char wifiInitialApPassword[] = "mrc4president";   // Initial password, used when it creates its own Access Point

// Device configuration
char cfgMqttServer[STRING_LEN];
char cfgMqttPort[NUMBER_LEN];
char cfgDeviceId[STRING_LEN];
char cfgDeviceName[STRING_LEN];

// Node 1 configuration
char cfgservo1aMin[NUMBER_LEN];
char cfgservo1aMax[NUMBER_LEN];
char cfgservo1aMove[NUMBER_LEN];
char cfgservo1aBack[NUMBER_LEN];
char cfgServo1bMin[NUMBER_LEN];
char cfgServo1bMax[NUMBER_LEN];
char cfgServo1bMove[NUMBER_LEN];
char cfgServo1bBack[NUMBER_LEN];

// Node 2 configuration
char cfgservo2aMin[NUMBER_LEN]="85";
char cfgservo2aMax[NUMBER_LEN];
char cfgservo2aMove[NUMBER_LEN];
char cfgservo2aBack[NUMBER_LEN];
char cfgServo2bMin[NUMBER_LEN];
char cfgServo2bMax[NUMBER_LEN];
char cfgServo2bMove[NUMBER_LEN];
char cfgServo2bBack[NUMBER_LEN];

// Indicate if it is time to reset the client or connect to MQTT
boolean needMqttConnect = false;
// boolean needReset = false;

// When CONFIG_PIN is pulled to ground on startup, the client will use the initial
// password to buld an AP. (E.g. in case of lost password)
// #define CONFIG_PIN D8

// Status indicator pin.
// First it will light up (kept LOW), on Wifi connection it will blink
// and when connected to the Wifi it will turn off (kept HIGH).
// #define STATUS_PIN LED_BUILTIN


// -------------------------------------------------------------------------------------------------
// IotWebConf configuration

// Callback method declarations
void wifiConnected();
void configSaved();

// Start DNS, Webserver & IotWebConf
DNSServer dnsServer;
WebServer server(80);
IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);

// Define settings to show up on configuration web page - Device
IotWebConfParameter webDeviceId = IotWebConfParameter("Enhetens unika id", "deviceId", cfgDeviceId, STRING_LEN);
IotWebConfParameter webDeviceName = IotWebConfParameter("Enhetens namn", "deviceName", cfgDeviceName, STRING_LEN);

// Add your own configuration - MQTT
IotWebConfParameter webMqttServer = IotWebConfParameter("MQTT IP-adress", "mqttServer", cfgMqttServer, STRING_LEN);
IotWebConfParameter webMqttPort = IotWebConfParameter("MQTT-port", "mqttPort", cfgMqttPort, NUMBER_LEN);

// Separator with text
IotWebConfSeparator separator1a = IotWebConfSeparator("Inst&auml;llningar f&ouml;r Servo 1a");

// Define settings to show up on configuration web page - Servo 1a
IotWebConfParameter webservo1aMin = IotWebConfParameter("Servo 1a min", "servo1aMin", cfgservo1aMin, NUMBER_LEN);
IotWebConfParameter webservo1aMax = IotWebConfParameter("Servo 1a max", "servo1aMax", cfgservo1aMax, NUMBER_LEN);
IotWebConfParameter webservo1aMove = IotWebConfParameter("Servo 1a hastighet", "servo1aMove", cfgservo1aMove, NUMBER_LEN);
IotWebConfParameter webservo1aBack = IotWebConfParameter("Servo 1a back", "servo1aBack", cfgservo1aBack, NUMBER_LEN);

// Separator with text
IotWebConfSeparator separator1b = IotWebConfSeparator("Inst&auml;llningar f&ouml;r Servo 1b");

// Define settings to show up on configuration web page - Servo 1b
IotWebConfParameter webServo1bMin = IotWebConfParameter("Servo 1b min", "servo1bMin", cfgServo1bMin, NUMBER_LEN);
IotWebConfParameter webServo1bMax = IotWebConfParameter("Servo 1b max", "servo1bMax", cfgServo1bMax, NUMBER_LEN);
IotWebConfParameter webServo1bMove = IotWebConfParameter("Servo 1b hastighet", "servo1bMove", cfgServo1bMove, NUMBER_LEN);
IotWebConfParameter webServo1bBack = IotWebConfParameter("Servo 1b back", "servo1bBack", cfgServo1bBack, NUMBER_LEN);

// Separator with text
IotWebConfSeparator separator2a = IotWebConfSeparator("Inst&auml;llningar f&ouml;r Servo 2a");

// Define settings to show up on configuration web page - Servo 2a
IotWebConfParameter webservo2aMin = IotWebConfParameter("Servo 2a min", "servo2aMin", cfgservo2aMin, NUMBER_LEN);
IotWebConfParameter webservo2aMax = IotWebConfParameter("Servo 2a max", "servo2aMax", cfgservo2aMax, NUMBER_LEN);
IotWebConfParameter webservo2aMove = IotWebConfParameter("Servo 2a hastighet", "servo2aMove", cfgservo2aMove, NUMBER_LEN);
IotWebConfParameter webservo2aBack = IotWebConfParameter("Servo 2a back", "servo2aBack", cfgservo2aBack, NUMBER_LEN);

// Separator with text
IotWebConfSeparator separator2b = IotWebConfSeparator("Inst&auml;llningar f&ouml;r Servo 2b");

// Define settings to show up on configuration web page - Servo 1b
IotWebConfParameter webServo2bMin = IotWebConfParameter("Servo 2b min", "servo2bMin", cfgServo2bMin, NUMBER_LEN);
IotWebConfParameter webServo2bMax = IotWebConfParameter("Servo 2b max", "servo2bMax", cfgServo2bMax, NUMBER_LEN);
IotWebConfParameter webServo2bMove = IotWebConfParameter("Servo 2b hastighet", "servo2bMove", cfgServo2bMove, NUMBER_LEN);
IotWebConfParameter webServo2bBack = IotWebConfParameter("Servo 2b back", "servo2bBack", cfgServo2bBack, NUMBER_LEN);


void wifiSetup() {
  // ------------------------------------------------------------------------------------------------
  // IotWebConfig start
  if (debug == 1) {Serial.println(dbText+"IotWebConf start");}

  // Adding up items to show on config web page
  iotWebConf.addParameter(&webDeviceId);
  iotWebConf.addParameter(&webDeviceName);
  iotWebConf.addParameter(&webMqttServer);
  iotWebConf.addParameter(&webMqttPort);
  iotWebConf.addParameter(&separator1a);
  iotWebConf.addParameter(&webservo1aMin);
  iotWebConf.addParameter(&webservo1aMax);
  iotWebConf.addParameter(&webservo1aMove);
  iotWebConf.addParameter(&webservo1aBack);
  iotWebConf.addParameter(&separator1b);
  iotWebConf.addParameter(&webServo1bMin);
  iotWebConf.addParameter(&webServo1bMax);
  iotWebConf.addParameter(&webServo1bMove);
  iotWebConf.addParameter(&webServo1bBack);
  iotWebConf.addParameter(&separator2a);
  iotWebConf.addParameter(&webservo2aMin);
  iotWebConf.addParameter(&webservo2aMax);
  iotWebConf.addParameter(&webservo2aMove);
  iotWebConf.addParameter(&webservo2aBack);
  iotWebConf.addParameter(&separator2b);
  iotWebConf.addParameter(&webServo2bMin);
  iotWebConf.addParameter(&webServo2bMax);
  iotWebConf.addParameter(&webServo2bMove);
  iotWebConf.addParameter(&webServo2bBack);
  iotWebConf.getApTimeoutParameter()->visible = true; // Show & set AP timeout at start

//  iotWebConf.setStatusPin(STATUS_PIN);
//  iotWebConf.setConfigPin(CONFIG_PIN);

  iotWebConf.setConfigSavedCallback(&configSaved);
  iotWebConf.setWifiConnectionCallback(&wifiConnected);
  
  // -- Get all the configuration/settings from EEPROM memory
  // TODO Hantera/konvertera MQTT-parametrar
  boolean validConfig = iotWebConf.init();
  if (validConfig == true) {
    if (debug == 1) {Serial.println(dbText+"Config from EEPROM");}
    deviceID = String(cfgDeviceId);
    deviceName = String(cfgDeviceName);

    servo1aMin = atoi(cfgservo1aMin);
    servo1aMax = atoi(cfgservo1aMax);
    servo1aMove = atoi(cfgservo1aMove);
    servo1aBack = atoi(cfgservo1aBack);
    servo1bMin = atoi(cfgServo1bMin);
    servo1bMax = atoi(cfgServo1bMax);
    servo1bMove = atoi(cfgServo1bMove);
    servo1bBack = atoi(cfgServo1bBack);

    servo2aMin = atoi(cfgservo2aMin);
    servo2aMax = atoi(cfgservo2aMax);
    servo2aMove = atoi(cfgservo2aMove);
    servo2aBack = atoi(cfgservo2aBack);
    servo2bMin = atoi(cfgServo2bMin);
    servo2bMax = atoi(cfgServo2bMax);
    servo2bMove = atoi(cfgServo2bMove);
    servo2bBack = atoi(cfgServo2bBack);

} else {
    if (debug == 1) {Serial.println(dbText+"Default config");}
    deviceName = String(thingName);
    String tmpNo = String(random(2147483647));
    deviceID = deviceName+"-"+tmpNo;

    servo1aMin = 80;
    servo1aMax = 94;
    servo1aMove = 100;
    servo1aBack = 3;
    servo1bMin = 87;
    servo1bMax = 99;
    servo1bMove = 100;
    servo1bBack = 3;

    servo2aMin = 80;
    servo2aMax = 94;
    servo2aMove = 100;
    servo2aBack = 3;
    servo2bMin = 87;
    servo2bMax = 99;
    servo2bMove = 100;
    servo2bBack = 3;
}

  if (debug == 1) {Serial.println(dbText+"IotWebConf start...done");}

}

// --------------------------------------------------------------------------------------------------
//  Function to show AP (access point) web start page
// --------------------------------------------------------------------------------------------------
void handleRoot()
{
  // Convert AP timout time från microseconds to seconds for better readability
  int ApTimeout = iotWebConf.getApTimeoutMs()/1000;
  
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String page = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" ";
  page += "content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  page += "<title>MRC-inst&auml;llningar</title></head><body>";
  page += "<h1>Inst&auml;llningar</h1>";
  page += "<p>V&auml;lkommen till MRC-enheten med namn: '";
  page += cfgDeviceId;
  page += "'</p>";
  page += "P&aring; sidan <a href='config'>Inst&auml;llningar</a> ";
  page += "kan du best&auml;mma hur just denna MRC-klient ska fungera.";

  page += "<p>M&ouml;jligheten att &auml;ndra dessa inst&auml;llningar &auml;r ";
  page += "alltid tillg&auml;nglig de f&ouml;rsta ";
  page += ApTimeout;
  page += " sekunderna efter start av enheten.";
  page += "</body></html>\n";

  server.send(200, "text/html", page);
}


// --------------------------------------------------------------------------------------------------
//  Function beeing called when wifi connection is up and running
// --------------------------------------------------------------------------------------------------
void wifiConnected() {
  if (debug == 1) {Serial.println(dbText+"Wifi connected, start MQTT");}
  // We are ready to start the MQTT connection
  needMqttConnect = true;
}
