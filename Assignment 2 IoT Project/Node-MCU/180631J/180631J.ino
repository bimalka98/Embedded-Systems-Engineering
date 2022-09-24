#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager


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

}

void loop() {
  // put your main code here, to run repeatedly:
}
