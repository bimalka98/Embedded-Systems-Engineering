/*
*************************************************************************************
*                                Author Details                                     *
* Author: Thalagala B. P.   180631J                                                 *
* Github: https://github.com/bimalka98                                              *
* Email: bimalkapiyaruwan1998322@gmail.com                                          *
*===================================================================================*
*/

// include the necessary libraries 
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient


// initialize the global variables
WiFiClient ESPClient;
PubSubClient mqttClient(ESPClient);


// user defined fucntion declaration
void receiveCallback(char * topic, byte* payload, unsigned int length);
void connectToBroker();
/*
*************************************************************************************
*                >>>       Start of Setup Function       >>>                        *
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
  res = wm.autoConnect("NodeMCU", "12345678"); // password protected ap on NodeMCU

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

}

/*
*************************************************************************************
*                     >>>         Start of Loop         >>>                         *
*************************************************************************************
*/

void loop() {
  // main code, to run repeatedly:
  if(!mqttClient.connected()) {
    connectToBroker();
  }

  // checking any incoming messages for callback
  mqttClient.loop();
  delay(1000);
}

/*
*************************************************************************************
*            >>>             User Defined Functions           >>>                   *
*************************************************************************************
*/

void receiveCallback(char * topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("[INFO] message received [");
  Serial.print(topic);
  Serial.print("] ");

  for(int i= 0; i < length; i++){
    Serial.print((char)payload[i]);
  }
}

void connectToBroker() {
  while(! mqttClient.connected()){
    Serial.println("[INFO] attempting MQTT connection...");

    if(mqttClient.connect("ESP-8266-180631J")){ // just a random string for authentication purposes
      Serial.println("[INFO] connected to MQTT broker.");
      mqttClient.subscribe("180631J");      
    }else{
      Serial.println("[ERROR] failed to connect.");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}