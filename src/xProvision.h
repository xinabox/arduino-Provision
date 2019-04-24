#ifndef xProvision_h
#define xProvision_h

#ifdef  ARDUINO_ARCH_ESP32
    #include    "FS.h"
    #include    "WiFi.h"
    #include    "SPIFFS.h"
    #define     LED_RED 25
    #define     LED_GREEN 26
    #define     LED_BUILTIN 27
#elif   ARDUINO_ARCH_ESP8266
    #include    "FS.h"
    #include    "ESP8266WiFi.h"
    #define     LED_RED 12
    #define     LED_GREEN 13
    #define     LED_BUILTIN 5
#else 
    #error  This Library only provides support for the CW01(ESP8266) and CW02(ESP32). 
    #error  Please select the correct board from the Menu -> Tools -> Board.
#endif

#include <ArduinoJson.h>
#include <Arduino.h>

#define BAUDSPEED 115200
#define SYNC "$!$"
#define PROV_VERSION "1.0.7"

class xProvision
{
    public:
        xProvision();
        ~xProvision();
        bool begin(void);
        void reset(void);
        void addWiFi(void);
        void addCloudToken(void);
        void addUbiDotsToken(void);
        void addBlynkToken(void);
        void addAzureToken(void);
        void addMQTT(void);
        void optionBlinkLED(void);
        void printConfigJson(void);
        void transmit(void);
        bool receive(void);
        bool success(void);
        bool getWiFi(String &ssid, String &psk);
        bool getMQTT(String &mqtt, String &port);
        bool getUbiDotsToken(String &var1);
        bool getAzureToken(String &var1);
        bool getBlynkToken(String &var1);
        bool getCloudToken(String &var1);
        void enableLED(void);
        void BlinkLED(uint32_t _delay);
        void BlinkLED(void);
        void fail(void);
        void end(void);
        void end(String var);
        void addVariable(String var1, String var2);
        bool getVariable(String var1, String &var2);
        void addCustomJson(String var1);
        bool saveConfigFile(void);
        bool saveConfigFile(String json);
        bool loadConfigFile(String &var1);
        bool formatMemory(void);
        bool createConfigFile(void);
        void retransmit(void);
    private:
        uint8_t tick_prov_wait = 0;
        String jsonFile, jsonFileStored;
        uint16_t total = 0;
        String _ssid, _pwd, _ubidots_token, _mqtt_server, _mqtt_port, _azure_token, _blynk_token, _cloud_token, _led;
        uint8_t blinkLED;
        bool provision_successful = false, config_file_exists = false;
        void merge_json(String obj1, String obj2);
        void buildConfigJson(String var);
        bool store_data(String json);
        bool load_data(String &json);
        bool writeFile(String json);
        bool readFile(String &json); 
        bool CHECK_XINABOX_JSON(String var1, String &var2);
        bool CREATE_XINABOX_JSON(void);
};
#endif