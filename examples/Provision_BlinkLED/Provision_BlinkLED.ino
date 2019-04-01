/*
    This sketch shows users how to use the provisioning library
    Examples show how to enable the Blinking LED
*/

#include <ArduinoJson.h>    // https://github.com/bblanchon/ArduinoJson
#include <xProvision.h>     // https://github.com/xinabox/arduino-Provision    

xProvision prv;
String ssid, password;

void setup() {
  // put your setup code here, to run once:
  prv.begin();
  prv.optionBlinkLED();

  prv.transmit();
  //Writes this string, with the CRLF and SYNC prefixed and CRLF suffixed
  //$!${""LED_to_blink":["Red","Green","Blue"]}
  //A nested array appears as a drop down list, the rest as text entry.

  prv.recieve();
  //Reads this string, with CRLF suffixed
  //{"LED_to_blink":"Red"}
  //Obvisouly with other data inserted, notice the dropdown comes back as the selected value.

  if (prv.success())
  {
    prv.enableLED();
  }
}

void loop()
{
  prv.BlinkLED();
}