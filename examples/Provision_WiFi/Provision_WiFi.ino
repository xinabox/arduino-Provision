/*
    This sketch shows users how to use the provisioning library for WiFi
*/

#include <ArduinoJson.h>    // https://github.com/bblanchon/ArduinoJson
#include <xProvision.h>     // https://github.com/xinabox/arduino-Provision    

xProvision prv;
String ssid, password;

void setup() {
  // Set RGB Pins as outputs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // put your setup code here, to run once:
  prv.begin();
  prv.addWiFi();

  prv.transmit();
  //Writes this string, with the CRLF and SYNC prefixed and CRLF suffixed
  //$!${"WiFi_Network":"Your SSID","WiFi_Password":"Your PSK"}

  prv.receive();
  //Reads this string, with CRLF suffixed
  //{"WiFi_Network":"Your-SSID","WiFi_Password":"Your-PSK"}

  if (prv.success())
  {
    prv.getWiFi(ssid, password);
    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_RED, HIGH);
      delay(250);
      digitalWrite(LED_RED, LOW);
      delay(250);
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_GREEN, HIGH);
  }
  else
  {
    prv.fail();
  }
}

void loop()
{

}