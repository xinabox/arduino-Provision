#include <xProvision.h>

/**
 * @brief Construct a new xProvision::xProvision object
 * 
 */
xProvision::xProvision(void)
{
    jsonFileName = "xinaboxuploader.json";
}

/**
 * @brief Construct a new x Provision::x Provision object
 * 
 * @param FileName 
 */
xProvision::xProvision(String FileName)
{
    jsonFileName = FileName;
}

/**
 * @brief Destroy the xProvision::xProvision object
 * 
 */
xProvision::~xProvision(void)
{
    /*nothing to destruct*/
}

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool xProvision::begin(void)
{
    Serial.begin(BAUDSPEED);
    while (!Serial)
        continue;
    Serial.println();
    return loadConfigFile(jsonFileStored);
}

/**
 * @brief checks if there is an old config file to present to xinaboxuploader
 * 
 */
void xProvision::retransmit(void)
{
    if (config_file_exists)
    {
        if (strcmp(jsonFileStored.c_str(), jsonFile.c_str()) != 0)
        {
            // #ifdef DEBUG_SERIAL
            // Serial.print("jsonFileStored - ");
            // Serial.println(jsonFileStored);
            // #endif

            if ((jsonFile[0] != '{') || (strncmp(jsonFileStored.c_str(), jsonFile.c_str(), 5) == 0))
            {
                jsonFile = "";
                merge_json(jsonFile, jsonFileStored);
            }
        }
    }
}

/**
 * @brief send data to xinaboxuploader
 * 
 */
void xProvision::transmit(void)
{
    retransmit();
    Serial.print(SYNC);
    printConfigJson();
}

/**
 * @brief recieve data from xinaboxuploader
 * 
 * @return true data recieved with no errors
 * @return false data not recieved or errors
 */
bool xProvision::receive(void)
{
    unsigned long currentTime = millis();
    uint8_t tick_prov_wait = 0;

    while ((!Serial.available()) && (tick_prov_wait < 40))
    {
        if ((millis() - currentTime) > 250)
        {
            currentTime = millis();
            tick_prov_wait++;
            debugPrint("wait timer - ", tick_prov_wait);
        }
    }
    if (tick_prov_wait == 40)
    {
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.println("Provision Ended, Received Nothing!");
        return false;
    }
    else
    {
        //Receive String to indicate provisioning will occur
        //while (!Serial.available());
        
        String syncData = "";
        syncData = Serial.readStringUntil('\n');
        syncData += '\0';

        if (strncmp(syncData.c_str(), "$!$$!$START", 11) == 0)
        {
            debugPrint("Ready to Recieve");

            while (!Serial.available());

            String jsonInputStringUser = Serial.readStringUntil('\n');
            provision_successful = true;
            //Serial.println(jsonInputStringUser);
            return saveConfigFile(jsonInputStringUser);
        }
        else
        {
            Serial.println(syncData);
            Serial.print(SYNC); //Part of the provisioning standard
            Serial.print(SYNC); //Part of the provisioning standard
            Serial.println("Provision Ended, Bad Sync!");
            return false;
        }
    }
}

/**
 * @brief prints provision failed to xinaboxuploader
 * 
 */
void xProvision::fail(void)
{
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print("Provision Failed.");
}

/**
 * @brief function to check for succesful provision
 * 
 * @return true 
 * @return false 
 */
bool xProvision::success(void)
{
    return provision_successful;
}

/**
 * @brief templete to add led option to blink
 * 
 */
void xProvision::optionBlinkLED(void)
{
    StaticJsonDocument<256> doc;
    JsonArray data = doc.createNestedArray("LED_to_blink");
    data.add("Red");
    data.add("Green");
    data.add("Blue");

    char json[256];
    char _json[256];
    size_t n = serializeJson(doc, json);

    for (int i = 0; i <= n; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[n - 2] = '\0';
    //debugPrint(_json);
    buildConfigJson(_json);
}

/**
 * @brief enable the LED selected from xinaboxuploader
 * 
 */
void xProvision::enableLED(void)
{
    pinMode(blinkLED, OUTPUT);
}

/**
 * @brief 
 * 
 */
void xProvision::BlinkLED(void)
{
    BlinkLED(1000);
}

/**
 * @brief 
 * 
 * @param _delay 
 */
void xProvision::BlinkLED(uint32_t _delay)
{
    digitalWrite(blinkLED, HIGH);
    delay(_delay);
    digitalWrite(blinkLED, LOW);
    delay(_delay);
}

/**
 * @brief print the current config file
 * 
 */
void xProvision::printConfigJson(void)
{
    char prov[total + 5];
    for (int i = 0; i < (total + 1); i++)
    {
        prov[i] = jsonFile[i];
    }
    Serial.println(prov);
}

/**
 * @brief 
 * 
 */
void xProvision::end(void)
{
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print("Provision Completed Successfully.");
}

/**
 * @brief 
 * 
 * @param var 
 */
void xProvision::end(String var)
{
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print(var);
}

/**
 * @brief add custrom named variable
 * 
 * @param var1 data name
 * @param var2 data initial condition
 */
void xProvision::addVariable(String var1, String var2)
{
    StaticJsonDocument<256> doc;
    doc[var1] = var2;

    char json[256];
    char _json[256];
    size_t n = serializeJson(doc, json);

    for (int i = 0; i <= n; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[n - 2] = '\0';
    //debugPrint(_json);
    buildConfigJson(_json);
}

/**
 * @brief get custom user variable data
 * 
 * @param var1 custom data name
 * @param var2 value of custom data
 * @return true 
 * @return false 
 */
bool xProvision::getVariable(String var1, String &var2)
{
    //debugPrint("getVariable()");
    if (loadConfigFile(jsonFileStored))
    {
        return checkConfigDataForVariable(var1, var2);
    }
    //Serial.println("File load fail");
    return false;
}

/**
 * @brief Add a custom json string for advvanced configs
 * 
 * @param var1 
 */
void xProvision::addCustomJson(String var1)
{
    const size_t size = sizeof(var1);
    DynamicJsonDocument doc(size);
    DeserializationError error = deserializeJson(doc, var1);

    char json[size];
    char _json[size];
    size_t n = serializeJson(doc, json);

    for (int i = 0; i <= n; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[n - 2] = '\0';
    //debugPrint(_json);
    buildConfigJson(_json);
}

/**
 * @brief build new config file from two json files
 * 
 * @param _json file to merge
 */
void xProvision::buildConfigJson(String _json)
{
    merge_json(jsonFile, _json);
}

/**
 * @brief merge two json files
 * 
 * @param obj1 json file 1 
 * @param obj2 json file 2
 */
void xProvision::merge_json(String obj1, String obj2)
{
    // debugPrint("MergeDocs");
    // debugPrint(obj1);
    // debugPrint(obj2);

    uint16_t len1 = strlen(obj1.c_str());
    uint16_t len2 = strlen(obj2.c_str());

    if (obj1[0] == '{')
    {
        for (uint16_t pos = 0; pos < len1; pos++)
        {
            obj1[pos] = obj1[pos + 1];
        }
        obj1[len1 - 2] = '\0';
        len1 = strlen(obj1.c_str());
    }

    if (obj1[0] == '{' && obj1[1] == '}')
    {
        //debugPrint("FixNewJson");
    }

    if (obj2[0] == '{')
    {
        for (uint16_t pos = 0; pos < len2; pos++)
        {
            obj2[pos] = obj2[pos + 1];
        }
        obj2[len2 - 2] = '\0';
        len2 = strlen(obj2.c_str());
    }

    total = len1 + len2 + 8;

    // merge two json
    if ((obj1[0] == '\"') && (obj2[0] == '\"'))
    {
        char merged[total];
        memset(merged, '\0', sizeof(merged));
        jsonFile = "\0";

        merged[0] = '{';

        for (int i = 1; i < (len1 + 1); i++)
        {
            merged[i] = obj1[i - 1];
        }

        merged[len1 + 1] = ',';

        for (int j = (len1 + 2); j < (len1 + len2 + 2); j++)
        {
            merged[j] = obj2[j - len1 - 2];
        }

        merged[len1 + len2 + 2] = '}';
        jsonFile = merged;
    }
    // merge single json to empty string
    else if ((strlen(obj1.c_str()) == 0) && (obj2[0] == '\"'))
    {
        char merged[total];
        memset(merged, '\0', sizeof(merged));

        merged[0] = '{';

        for (int i = 1; i < (len2 + 1); i++)
        {
            merged[i] = obj2[i - 1];
        }
        merged[len2 + 1] = '}';

        jsonFile = merged;
    }
    else
    {
        Serial.print(SYNC);
        Serial.print(SYNC);
        Serial.println("Error in JSON");
    }
}

/**
 * @brief reset the current config file
 * 
 */
void xProvision::reset(void)
{
    jsonFile = jsonFileStored = "";
}

/**
 * @brief save current config file to FS
 * 
 * @return true 
 * @return false 
 */
bool xProvision::saveConfigFile(void)
{
    if (strcmp(jsonFileStored.c_str(), jsonFile.c_str()) != 0)
    {
        return saveFile(jsonFile);
    }
    return true;
}

/**
 * @brief save custom config file
 * 
 * @param json custom user config file
 * @return true 
 * @return false 
 */
bool xProvision::saveConfigFile(String json)
{
    if (strcmp(jsonFileStored.c_str(), json.c_str()) != 0)
    {
        return !errorHandlers(saveFile(json));
    }
    return true;
}

/**
 * @brief load config file from FS
 * 
 * @param var1 to store cofnig data inn
 * @return true 
 * @return false 
 */
bool xProvision::loadConfigFile(String &var1)
{
    return !errorHandlers(loadFile(var1));
}

/**
 * @brief write config file to FS
 * 
 * @param json config file to write to FS
 * @return true 
 * @return false 
 */
uint8_t xProvision::saveFile(String userJson)
{
    //debugPrint(userJson);
    if (SPIFFS.begin()) // start the FS
    {
        File xinaboxFile = SPIFFS.open(jsonFileName, "w");

        if (xinaboxFile)
        {
            size_t sizeJson = strlen(userJson.c_str());
            char json[sizeJson];
            strcpy(json, userJson.c_str());
            DynamicJsonDocument doc(sizeJson * 2);
            DeserializationError error = deserializeJson(doc, userJson);
            if (!error)
            {
                if (serializeJson(doc, xinaboxFile) == 0)
                {
                    debugPrint("Failed to write to file");
                    return 0x24;
                }
                xinaboxFile.close();
                SPIFFS.end();
                //debugPrint("SAVE DONE");
                return 0xFF;                
            }
            else
            {
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.print("deserializeJson() failed: ");
                Serial.print(error.c_str());
                return 0x23;
            }
        }
        else
        {
            Serial.print(SYNC); //Part of the provisioning standard
            Serial.print(SYNC); //Part of the provisioning standard
            Serial.println("Failed to open xinaboxuploader.json");
            return 0x22;
        }
    }
    else
    {
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.println("Device Error, Rebooting xChip. Please Flash again.");
        SPIFFS.format();
        SPIFFS.end();
        return 0x20;
    }
}

/**
 * @brief read current config file from DA
 * 
 * @param json to store config data in
 * @return true 
 * @return false 
 */
uint8_t xProvision::loadFile(String &json)
{
    json = "";
    
    if (SPIFFS.begin())
    {
        if (SPIFFS.exists(jsonFileName)) 
        {
            config_file_exists = true;
            File xinaboxFile = SPIFFS.open(jsonFileName, "r");
            if (xinaboxFile)
            {
                size_t size = xinaboxFile.size();
            
                std::unique_ptr<char[]> buf1(new char[size]);
                xinaboxFile.readBytes(buf1.get(), size);
                DynamicJsonDocument doc(size);
                DeserializationError error = deserializeJson(doc, buf1.get());
                xinaboxFile.close();
                //config_file_exists = true;
                //size_t len = serializeJson(doc, json);

                if (!error)
                {
                    if (serializeJson(doc, json) == 0)
                    {
                        debugPrint("Failed to write to file");
                        return 0x14;
                    }

                    // #ifdef DEBUG_SERIAL
                    // Serial.print("loadFile() - ");
                    // Serial.println(json);
                    // #endif

                    xinaboxFile.close();
                    SPIFFS.end();
                    provision_successful = true;
                    config_file_exists = true;
                    //debugPrint("LOAD DONE");
                    return 0xFF;
                }
                else
                {
                    Serial.print(SYNC); //Part of the provisioning standard
                    Serial.print(SYNC); //Part of the provisioning standard
                    Serial.println("Parsing of config file failed");
                    return 0x13;
                }
            }
            else
            {
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.println("Failed to open config file");
                return 0x12;
            }
        }
        else
        {
            SPIFFS.end();
            config_file_exists = false;
            //errorHandlers(createJsonConfigFile());
            return 0x11;
        }
    }
    else
    {
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.println("Reboot xChip. Please Flash again.");
        SPIFFS.format();
        SPIFFS.end();
        return 0x10;
    }
}

/**
 * @brief create a new config file
 * 
 * @return true 
 * @return false 
 */
bool xProvision::createConfigFile(void)
{
    return saveConfigFile();
}

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
uint8_t xProvision::createJsonConfigFile(void)
{
    //save the custom parameters to FS
    if (SPIFFS.begin())
    {
        //Serial.println(jsonFileName);
        File xinaboxFile = SPIFFS.open(jsonFileName, "w");
        if (xinaboxFile)
        {
            size_t n = sizeof(jsonFile);
            DynamicJsonDocument doc(n);
            DeserializationError error = deserializeJson(doc, jsonFile);
            debugPrint("JsonFile below:");
            debugPrint(jsonFile);

            if (!error)
            {
                //json1.prettyPrintTo(Serial);
                serializeJson(doc, xinaboxFile);
                xinaboxFile.close();
                SPIFFS.end();
                return 0xFF;
            }
            else
            {
                return 0x03;
            }
        }
        else
        {
            return 0x02;          
            Serial.print(SYNC); //Part of the provisioning standard
            Serial.print(SYNC); //Part of the provisioning standard
            Serial.println("Failed to create config file.");
            return false;
        }
    }
    else
    {
        return 0x01;
    }
}

/**
 * @brief 
 * 
 * @param var1 
 * @param var2 
 * @return true 
 * @return false 
 */
bool xProvision::checkConfigDataForVariable(String var1, String &var2)
{
    size_t sizeJson = strlen(jsonFileStored.c_str());
    char json[sizeJson];
    strcpy(json, jsonFileStored.c_str());
    DynamicJsonDocument doc(sizeJson * 2);
    DeserializationError err = deserializeJson(doc, jsonFileStored);
    JsonObject jsonObj = doc.as<JsonObject>();

    if (!err)
    {
        if (jsonObj.containsKey(var1))
        {
            var2 = jsonObj[var1].as<String>();
            SPIFFS.end();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        debugPrint("deserializeJson() failed: ");
        debugPrint(err.c_str());
        debugPrint("error occured");
        return false;
    }

    return false;
}

/**
 * @brief reset the FS on CW01/02
 * 
 * @return true 
 * @return false 
 */
bool xProvision::formatMemory(void)
{
    SPIFFS.format();
}

/**
 * @brief provide json file with custom name
 * 
 * @param filename 
 */
void xProvision::nameJsonFile(String filename)
{
    jsonFileName = filename;
    //debugPrint(jsonFileName);
}

/**
 * @brief 
 * 
 * @param _errCode 
 * @return uint8_t 
 */
bool xProvision::errorHandlers(uint8_t _errCode)
{
	switch (_errCode)
	{
        case 0x01: // 
            debugPrint("ERROR 0x01> createJsonConfigFile() SPIFFS FAIL");
            return true;
        case 0x02: // 
            debugPrint("ERROR 0x02> createJsonConfigFile() failed to create file");
            return true;
        case 0x03: // 
            debugPrint("ERROR 0x03> createJsonConfigFile() error with deserializeJson");
            return true;     
        case 0x10: // 
            debugPrint("ERROR 0x10> loadFile() SPIFFS FAIL");
            return true;
        case 0x11: // 
            debugPrint("ERROR 0x11> loadFile() json not found");
            return true;
        case 0x12: // 
            debugPrint("ERROR 0x12> loadFile() unable to open file");
            return true;
        case 0x13: // 
            debugPrint("ERROR 0x13> loadFile() jsonFile error");
            return true;
        case 0x14: // 
            debugPrint("ERROR 0x14> loadFile() unable to serialize");
            return true;                    
        case 0x20: // 
            debugPrint("ERROR 0x20> saveFile() SPIFFS FAIL");
            return true;
        case 0x21: // 
            debugPrint("ERROR 0x21> saveFile() json not found");
            return true;
        case 0x22: // 
            debugPrint("ERROR 0x22> saveFile() unable to open file");
            return true;
        case 0x23: // 
            debugPrint("ERROR 0x23> saveFile() jsonFile error");
            return true;
        case 0x24: // 
            debugPrint("ERROR 0x24> saveFile() unable to serialize");
            return true; 
        case 0x30: // 
            debugPrint("ERROR 0x30> ");
            return true;                
        case 0x31: // 
            debugPrint("ERROR 0x31> ");
            return true;
        case 0x32: //
            debugPrint("ERROR 0x32> ");
            return true;
        case 0x33: //
            debugPrint("ERROR 0x33> ");
            return true;
        case 0x34: // 
            debugPrint("ERROR 0x34> ");
            return true; 
        case 0x35: // 
            debugPrint("ERROR 0x35> ");
            return true;
        case 0x36: // 
            debugPrint("ERROR 0x36> ");
            return true;
        case 0x37: // 
            debugPrint("ERROR 0x37> ");
            return true;         
        case 0xFF:
            //debugPrint("RUN_OK");
            return false;
        default:
            return true;
	}    
}

/**
 * @brief 
 * 
 * @param _data 
 */
void xProvision::debugPrint(String _data)
{
#ifdef DEBUG_SERIAL
    Serial.print("xProvision: ");
	Serial.println(_data);
#endif
}

/**
 * @brief 
 * 
 * @param _data 
 * @param _num 
 */
void xProvision::debugPrint(String _data, uint16_t _num)
{
#ifdef DEBUG_SERIAL
    Serial.print("xProvision: ");
	Serial.print(_data);
	Serial.println(_num);
#endif
}