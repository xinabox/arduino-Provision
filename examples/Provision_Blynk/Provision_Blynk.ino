/*
    This sketch shows users how to use the provisioning library for Blynk
*/

#include <ArduinoJson.h>    // https://github.com/bblanchon/ArduinoJson
#include <xProvision.h>     // https://github.com/xinabox/arduino-Provision    
#include <BlynkSimpleEsp32.h> // https://github.com/blynkkk/blynk-library
xProvision prv;

String ssid, password, auth;

void setup() {
  // put your setup code here, to run once:
  prv.begin();
  prv.addWiFi();
  prv.addBlynkToken();
  prv.transmit();
  //Writes this string, with the CRLF and SYNC prefixed and CRLF suffixed
  //$!${"WiFi_Network":"Your SSID","WiFi_Password":"Your PSK","Blynk_Token":"Long odd number"}

  prv.recieve();
  //Reads this string, with CRLF suffixed
  //{"WiFi_Network":"WiFi SSID","WiFi_Password":"WiFi PSK","Blynk_Token":"your blynk token"}

  if (prv.success())
  {
    prv.getWiFi(ssid, password);
    prv.getBlynkToken(auth);
  }

  Blynk.begin(auth.c_str(), ssid.c_str(), password.c_str());

}

void loop()
{
  Blynk.run();
}