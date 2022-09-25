 /*
*************************************************************************************
                                 Author Details
  Author: Thalagala B. P.   180631J
  Github: https://github.com/bimalka98
  Email: bimalkapiyaruwan1998322@gmail.com
  ===================================================================================
*/

// include the necessary libraries
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient


// initialize the global variables
WiFiClient ESPClient;
PubSubClient mqttClient(ESPClient);
bool IsWarning = false; // viarible to store the weather condition normal/critical
String WarnInterval = ""; // interval between two led blinks at a warning


// user defined fucntion declaration
void receiveCallback(char * topic, byte* payload, unsigned int length);
void connectToBroker();

/*
*************************************************************************************
                 >>>       Start of Setup Function       >>>
*************************************************************************************
*/

void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  // setup code, to run once:
  Serial.begin(115200);

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
   wm.resetSettings();

  // Automatically connect using saved credentials if avilable,
  // if connection fails, starts an access point with the following credentials
  bool res;
  res = wm.autoConnect("NodeMCU", "12345678"); // password protected AP on NodeMCU

  // goes into a blocking loop awaiting configuration and will return success result
  if (!res) {
    Serial.println("[ERROR] Failed to connect");
  }
  else {
    //if you get here you have connected to the WiFi
    Serial.println("[INFO] connected to the external AP with internet access.");
  }

  // setup the mqtt client
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(receiveCallback);

  // for warning mechanism
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off at the beginning (LED is active low)
}

/*
*************************************************************************************
                      >>>         Start of Loop         >>>
*************************************************************************************
*/

void loop() {
  // main code, to run repeatedly:
  if (!mqttClient.connected()) {
    connectToBroker();
  }

  // checking any incoming messages for callback
  mqttClient.loop();

  // check whether a warning condition has occured
  if (IsWarning) {

    // if there's a waring blink the LED
    if (WarnInterval != "") {
      int warningDelay = WarnInterval.toInt(); // convert the string to an integer
      digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
      delay(100);                       // Wait for a some milisecond
      digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
      delay(warningDelay);              // Wait depending on the critical level
    }

  } else {

    // if weather conditions are normal simply Wait
    delay(1000);

  }
}

/*
*************************************************************************************
             >>>             User Defined Functions           >>>
*************************************************************************************
*/

void receiveCallback(char * topic, byte* payload, unsigned int length) {
  
  Serial.println();
  Serial.print("[INFO] message received [");
  Serial.print(topic);
  Serial.print("] ");

  // flow will be sending 'f' if the weather conditions are normal
  if (payload[0] == 'f') {

    // no warning condition
    IsWarning = false;
    Serial.print("normal condition.");

  // if any weather condition exceeds its threshold, an interval in miliseconds,
  // will be sent here to wait in between two LED blinks.
  // waiting interval is calculated according to the given logic in the Assignmenet
  // and the value is shown in miliseconds at the end of following message
  } else {

    // warning condition
    Serial.print("exceeding threshold... ");
    IsWarning = true;

    // extracting and visualising the waiting interval
    WarnInterval = ""; // clear the previous interval, if any

    for (int i = 0; i < length; i++) {
      
      // printing the interval between two blinks
      Serial.print((char)payload[i]); 

      // constructing the interval between two blinks, concatenating the received payload
      WarnInterval += (char)payload[i]; 

    }
  }

}


// fucntion copied completely form the IOT practical lecture
void connectToBroker() {
  while (! mqttClient.connected()) {
    Serial.println("[INFO] attempting MQTT connection...");

    if (mqttClient.connect("ESP-8266-180631J")) { // just a random string for authentication purposes
      Serial.println("[INFO] connected to MQTT broker.");
      mqttClient.subscribe("180631J");
    } else {
      Serial.println("[ERROR] failed to connect.");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}
