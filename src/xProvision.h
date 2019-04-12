
#ifndef xProvision_h
#define xProvision_h

#include <ArduinoJson.h>
#include <Arduino.h>

#ifdef  ARDUINO_ARCH_ESP32
#include    "WiFi.h"
#define     LED_RED 25
#define     LED_GREEN 26
#define     LED_BUILTIN 27
#elif   ARDUINO_ARCH_ESP8266
#include    <ESP8266WiFi.h>
#define     LED_RED 12
#define     LED_GREEN 13
#define     LED_BUILTIN 5
#else 
#error  This Library only provides support for the CW01(ESP8266), CW02(ESP32), CW03(ATSAMW25) and DW03(ATSAMW25). 
#error  Please select the correct board from the Menu.
#endif

#define BAUDSPEED 115200
#define SYNC "$!$"

class xProvision
{
    public:
        xProvision();
        ~xProvision();
        void begin(void);
        void addWiFi();
        void addCloudToken(void);
        void addUbiDotsToken(void);
        void addBlynkToken(void);
        void addAzureToken(void);
        void addMQTT(void);
        void optionBlinkLED(void);
        void printConfigJson(void);
        void transmit(void);
        bool recieve(void);
        bool success(void);
        bool getWiFi(String &var1, String &var2);
        bool getMQTT(String &var1, String &var2);
        bool getUbiDotsToken(String &var1);
        bool getAzureToken(String &var1);
        bool getBlynkToken(String &var1);
        bool getCloudToken(String &var1);
        void enableLED(void);
        void BlinkLED(uint32_t _delay);
        void BlinkLED(void);
        void fail(void);

    private:
        uint8_t tick_prov_wait = 0;
        String jsonFile;
        uint16_t total = 0;
        String _ssid, _pwd, _ubidots_token, _mqtt_server, _mqtt_port, _azure_token, _blynk_token, _cloud_token, _led;
        uint8_t blinkLED;
        bool provision_successful;
        void merge_json(String obj1, String obj2);
        void buildConfigJson(String var);
};
#endif