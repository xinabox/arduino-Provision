/*
 *  This sketch shows users how to use the provisioning library for WiFi
 */

#include <ArduinoJson.h>    // https://github.com/bblanchon/ArduinoJson
#include <xProvision.h>     // https://github.com/xinabox/arduino-Provision    

xProvision prv;
String ssid, password;

void setup() {
  // put your setup code here, to run once:
  prv.begin();
  prv.addWiFi();

  prv.transmit();
  //Writes this string, with the CRLF and SYNC prefixed and CRLF suffixed
  //$!${"WiFi_Network":"Your SSID","WiFi_Password":"Your PSK"}
  
  prv.recieve();
  //Reads this string, with CRLF suffixed
  //{"WiFi_Network":"WiFi SSID","WiFi_Password":"WiFi PSK"}

  if (prv.success())
  {
    prv.getWiFi(ssid, password);
  }

  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  
}