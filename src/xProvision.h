#ifndef xProvision_h
#define xProvision_h

#include <ArduinoJson.h>
#include <Arduino.h>

#ifdef  ARDUINO_ARCH_ESP32
    #include    "FS.h"
    #include    "SPIFFS.h"
    #define     LED_RED 25
    #define     LED_GREEN 26
    #define     LED_BUILTIN 27
#elif   ARDUINO_ARCH_ESP8266
    #include    "FS.h"
    #define     LED_RED 12
    #define     LED_GREEN 13
    #define     LED_BUILTIN 5
#else 
    #error  This Library only provides support for the CW01(ESP8266) and CW02(ESP32). 
    #error  Please select the correct board from the Menu -> Tools -> Board.
#endif

#ifndef ARDUINOJSON_VERSION
#error ArduinoJson not found, please include ArduinoJson.h in your .ino file
#endif

#if ARDUINOJSON_VERSION_MAJOR!=6 || ARDUINOJSON_VERSION_MINOR<1
#error ArduinoJson 6.1+ is required
#endif

#define BAUDSPEED 115200
#define SYNC "$!$"
#define PROV_VERSION "1.0.8"

#define DEBUG_SERIAL // used to enable serial prints

class xProvision
{
    public:
        /**
         * @brief Construct a new x Provision object
         * 
         */
        xProvision();

        /**
         * @brief Construct a new x Provision object
         * 
         * @param FileName 
         */
        xProvision(String FileName);

        /**
         * @brief Destroy the x Provision object
         * 
         */
        ~xProvision();

        /**
         * @brief 
         * 
         * @return true 
         * @return false 
         */
        bool begin(void);

        /**
         * @brief 
         * 
         */
        void reset(void);

        /**
         * @brief 
         * 
         */
        void optionBlinkLED(void);

        /**
         * @brief 
         * 
         */
        void printConfigJson(void);

        /**
         * @brief 
         * 
         */
        void transmit(void);

        /**
         * @brief 
         * 
         * @return true 
         * @return false 
         */
        bool receive(void);

        /**
         * @brief 
         * 
         * @return true 
         * @return false 
         */
        bool success(void);

        /**
         * @brief 
         * 
         */
        void enableLED(void);

        /**
         * @brief 
         * 
         * @param _delay 
         */
        void BlinkLED(uint32_t _delay);

        /**
         * @brief 
         * 
         */
        void BlinkLED(void);

        /**
         * @brief 
         * 
         */
        void fail(void);

        /**
         * @brief 
         * 
         */
        void end(void);

        /**
         * @brief 
         * 
         * @param var 
         */
        void end(String var);

        /**
         * @brief 
         * 
         * @param var1 
         * @param var2 
         */
        void addVariable(String var1, String var2);

        /**
         * @brief Get the Variable object
         * 
         * @param var1 
         * @param var2 
         * @return true 
         * @return false 
         */
        bool getVariable(String var1, String &var2);

        /**
         * @brief 
         * 
         * @param var1 
         */
        void addCustomJson(String var1);

        /**
         * @brief 
         * 
         * @return true 
         * @return false 
         */
        bool saveConfigFile(void);

        /**
         * @brief 
         * 
         * @param json 
         * @return true 
         * @return false 
         */
        bool saveConfigFile(String json);

        /**
         * @brief 
         * 
         * @param var1 
         * @return true 
         * @return false 
         */
        bool loadConfigFile(String &var1);

        /**
         * @brief 
         * 
         * @return true 
         * @return false 
         */
        bool formatMemory(void);

        /**
         * @brief Create a Config File object
         * 
         * @return true 
         * @return false 
         */
        bool createConfigFile(void);

        /**
         * @brief 
         * 
         */
        void retransmit(void);

        /**
         * @brief 
         * 
         * @param filename 
         */
        void nameJsonFile(String filename);

    private:
        String jsonFile, jsonFileStored, jsonFileName;
        uint16_t total = 0;
        uint8_t blinkLED;
        bool provision_successful = false, config_file_exists = false;

        /**
         * @brief 
         * 
         * @param obj1 
         * @param obj2 
         */
        void merge_json(String obj1, String obj2);

        /**
         * @brief 
         * 
         * @param var 
         */
        void buildConfigJson(String var);

        /**
         * @brief 
         * 
         * @param json 
         * @return true 
         * @return false 
         */
        uint8_t saveFile(String json);

        /**
         * @brief 
         * 
         * @param json 
         * @return true 
         * @return false 
         */
        uint8_t loadFile(String &json); 

        /**
         * @brief 
         * 
         * @param var1 
         * @param var2 
         * @return true 
         * @return false 
         */
        bool checkConfigDataForVariable(String var1, String &var2);

        /**
         * @brief Create a Json Config File object
         * 
         * @return true 
         * @return false 
         */
        uint8_t createJsonConfigFile(void);

		/**
		 * @brief 
		 * 
		 * @param _data 
		 */
		void debugPrint(String _data);

		/**
		 * @brief 
		 * 
		 * @param _data 
		 * @param _num 
		 */
		void debugPrint(String _data, uint16_t _num);

        /**
         * @brief 
         * 
         * @param _errCode 
         * @return uint8_t 
         */
        bool errorHandlers(uint8_t _errCode);
};
#endif