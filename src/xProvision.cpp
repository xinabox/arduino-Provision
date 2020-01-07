#include <xProvision.h>

xProvision::xProvision(void)
{
    /* nothing to construct*/
}

xProvision::~xProvision(void)
{
    /*nothing to destruct*/
}

bool xProvision::begin(void)
{
    Serial.begin(BAUDSPEED);
    while (!Serial)
        continue;
    Serial.println("START");

    // read config file
    return loadConfigFile(jsonFileStored);
}

void xProvision::retransmit(void)
{
    if(config_file_exists)
    {
        if(strcmp(jsonFileStored.c_str(), jsonFile.c_str()) != 0)
        {
            if((jsonFile[0] != '{') || (strncmp(jsonFileStored.c_str(), jsonFile.c_str(), 5) == 0))
            {
                jsonFile = "";
                merge_json(jsonFile, jsonFileStored);
            }            
        }
    }
}

void xProvision::transmit(void)
{
    retransmit();
    Serial.print(SYNC);
    printConfigJson();
}

bool xProvision::receive(void)
{
    //Wait for data
    unsigned long currentTime = 0;
    currentTime = millis();

    while ((!Serial.available()) && (tick_prov_wait < 100))
    {
        if ((millis() - currentTime) > 100)
        {
            currentTime = millis();
            tick_prov_wait++;
        }
    }

    if (tick_prov_wait == 100)
    {
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.println("Provision Ended, Received Nothing!");
        return false;
    }
    else
    {
        //Receive String to indicate provisioning will occur
        String start = Serial.readStringUntil('\n');

        if (strncmp(start.c_str(), "$!$$!$START", 11) == 0)
        {
            while (!Serial.available());
            String s = Serial.readStringUntil('\n');

            DynamicJsonBuffer jsonBuffer;
            JsonObject &root = jsonBuffer.parseObject(s);

            if (!root.success())
            {
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.println("Something went wrong!");
                return false;
            }
            else
            {
                if (root.containsKey("WiFi_Network") && root.containsKey("WiFi_Password"))
                {
                    _ssid = root["WiFi_Network"].as<String>();
                    _pwd = root["WiFi_Password"].as<String>();
                }

                if (root.containsKey("MQTT_Server") && root.containsKey("MQTT_Port"))
                {
                    _mqtt_server = root["MQTT_Server"].as<String>();
                    _mqtt_port = root["MQTT_Port"].as<String>();
                }

                if (root.containsKey("UbiDots_Token"))
                {
                    _ubidots_token = root["UbiDots_Token"].as<String>();
                }

                if (root.containsKey("Blynk_Token"))
                {
                    _blynk_token = root["Blynk_Token"].as<String>();
                }

                if (root.containsKey("Azure_Token"))
                {
                    _azure_token = root["Azure_Token"].as<String>();
                }

                if (root.containsKey("Cloud_Token"))
                {
                    _cloud_token = root["Cloud_Token"].as<String>();
                }

                if (root.containsKey("LED_to_blink"))
                {
                    _led = root["LED_to_blink"].as<String>();
                    switch (_led.charAt(0))
                    {
                    case 'R':
                        blinkLED = LED_RED;
                        break;
                    case 'G':
                        blinkLED = LED_GREEN;
                        break;
                    case 'B':
                        blinkLED = LED_BUILTIN;
                        break;
                    }
                }
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.print("Provision Completed Successfully.");
                provision_successful = true;

                return saveConfigFile(s);
            }
        }
        else
        {
            //digitalWrite(LED_GREEN, HIGH);
        }
    }
    return true;
}

void xProvision::fail(void)
{
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print("Provision Failed.");
}

bool xProvision::success(void)
{
    return provision_successful;
}

void xProvision::addWiFi(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["WiFi_Network"] = "YourSSID";
    root["WiFi_Password"] = "YourPSK";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= (int)size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getWiFi(String &ssid, String &psk)
{
    if ((_ssid[0] == '\0') || (_pwd[0] == '\0'))
    {
        return false;
    }
    ssid = _ssid;
    psk = _pwd;
    return true;
}

void xProvision::addMQTT(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["MQTT_Server"] = "ServerHostname";
    root["MQTT_Port"] = "ServerPort";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= (int)size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getMQTT(String &mqtt, String &port)
{
    if ((_mqtt_server[0] == '\0') || (_mqtt_port[0] == '\0'))
    {
        return false;
    }
    mqtt = _mqtt_server;
    port = _mqtt_port;
    return true;
}

void xProvision::addUbiDotsToken(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["UbiDots_Token"] = "YourUbitdotsToken";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= (int)size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getUbiDotsToken(String &var1)
{
    if (_ubidots_token[0] == '\0')
    {
        return false;
    }
    var1 = _ubidots_token;
    return true;
}

void xProvision::addAzureToken(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["Azure_Token"] = "YourAzureToken";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= (int)size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getAzureToken(String &var1)
{
    if (_azure_token[0] == '\0')
    {
        return false;
    }
    var1 = _azure_token;
    return true;
}

void xProvision::addBlynkToken(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["Blynk_Token"] = "YourBlynkToken";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= (int)size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getBlynkToken(String &var1)
{
    if (_blynk_token[0] == '\0')
    {
        return false;
    }
    var1 = _blynk_token;
    return true;
}

void xProvision::addCloudToken(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["Cloud_Token"] = "YourCloudToken";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= (int)size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getCloudToken(String &var1)
{
    if (_cloud_token[0] == '\0')
    {
        return false;
    }
    var1 = _cloud_token;
    return true;
}

void xProvision::optionBlinkLED(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    JsonArray &data = root.createNestedArray("LED_to_blink");
    data.add("Red");
    data.add("Green");
    data.add("Blue");

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= (int)size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

void xProvision::enableLED(void)
{
    pinMode(blinkLED, OUTPUT);
}

void xProvision::BlinkLED(void)
{
    BlinkLED(1000);
}

void xProvision::BlinkLED(uint32_t _delay)
{
    digitalWrite(blinkLED, HIGH);
    delay(_delay);
    digitalWrite(blinkLED, LOW);
    delay(_delay);
}

void xProvision::printConfigJson(void)
{
    char prov[total + 5];
    for (int i = 0; i < (total + 1); i++)
    {
        prov[i] = jsonFile[i];
    }
    Serial.println(prov);
}

void xProvision::end(void)
{
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print("Provision Completed Successfully.");
}

void xProvision::end(String var)
{
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print(var);
}

void xProvision::addVariable(String var1, String var2)
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root[var1] = var2;

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= (int)size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    buildConfigJson(_json);
}

bool xProvision::getVariable(String var1, String &var2)
{
    if (config_file_exists)
    {
        if(loadConfigFile(jsonFileStored))
        {
            return CHECK_XINABOX_JSON(var1, var2);
        }
        return false;
    }
    return false;
}

void xProvision::addCustomJson(String var1)
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(var1);

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= (int)size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    buildConfigJson(_json);
}

void xProvision::buildConfigJson(String _json)
{
    merge_json(jsonFile, _json);
}

void xProvision::merge_json(String obj1, String obj2)
{
    /*
    Serial.println("MergeDocs");
    Serial.println(obj1);
    Serial.println(obj2);
    */

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

    if(obj1[0] == '{' && obj1[1] == '}')
    {
        //Serial.println("FixNewJson");
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

void xProvision::reset(void)
{
    jsonFile = jsonFileStored = "";
}

bool xProvision::saveConfigFile(void)
{
    if(strcmp(jsonFileStored.c_str(), jsonFile.c_str()) != 0)
    {
        return store_data(jsonFile);
    }
    return true;
}

bool xProvision::saveConfigFile(String json)
{
    if(strcmp(jsonFileStored.c_str(), json.c_str()) != 0)
    {
        return store_data(json);
    }
    return true;
}

bool xProvision::loadConfigFile(String &var1)
{
    return load_data(var1);
}

bool xProvision::store_data(String json)
{
    return writeFile(json);
}

bool xProvision::load_data(String &json)
{
    return readFile(json);
}

bool xProvision::writeFile(String json)
{
    //File xinaboxFile = SPIFFS.open("/xinaboxuploader.json", "r");

    if (SPIFFS.begin())
    {
        //File xinaboxFile = SPIFFS.open("/xinaboxuploader.json", "r");

        if (SPIFFS.exists("/xinaboxuploader.json")) // search for xinaboxuploader.json
        { 
            config_file_exists = true; //file exists, reading and loading

            File xinaboxFile = SPIFFS.open("/xinaboxuploader.json", "w");

            if (xinaboxFile)
            {
                DynamicJsonBuffer jsonBuffer1;
                JsonObject &json1 = jsonBuffer1.parseObject(json);

                if (json1.success())
                {
                    //json1.prettyPrintTo(Serial);
                    json1.printTo(xinaboxFile);
                    xinaboxFile.close();
                    SPIFFS.end();
                    return true;
                }
                else
                {
                    Serial.print(SYNC); //Part of the provisioning standard
                    Serial.print(SYNC); //Part of the provisioning standard
                    Serial.println("Parsing of xinaboxuploader.json failed");
                    return false;
                }
            }
            else
            {
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.println("Failed to open xinaboxuploader.json");
                return false;
            }
        }
        else
        {
            SPIFFS.end();
            //CREATE_XINABOX_JSON();
            return false;
        }
    }
    else
    {
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.println("Device Error, Rebooting xChip. Please Flash again.");
        SPIFFS.format();
        //ESP.reset();
    }
    SPIFFS.end();
    return true;
}

bool xProvision::readFile(String &json)
{
    //File xinaboxFile = SPIFFS.open("/xinaboxuploader.json", "r");

    if (SPIFFS.begin())
    {
        //File xinaboxFile = SPIFFS.open("/xinaboxuploader.json", "r");

        if (SPIFFS.exists("/xinaboxuploader.json")) // search for xinaboxuploader.json
        {
            config_file_exists = true; //file exists, reading and loading

            File xinaboxFile = SPIFFS.open("/xinaboxuploader.json", "r");

            if (xinaboxFile)
            {
                size_t size = xinaboxFile.size();
                std::unique_ptr<char[]> buf1(new char[size]);
                xinaboxFile.readBytes(buf1.get(), size);
                DynamicJsonBuffer jsonBuffer1;
                JsonObject &json1 = jsonBuffer1.parseObject(buf1.get());
                xinaboxFile.close();
                size_t len = json1.measureLength();
                size_t sizeJson = len + 1;
                if (json1.success() && (sizeJson > 3))
                {
                    //json1.prettyPrintTo(Serial);
                    json1.printTo(json);
                    SPIFFS.end();
                    provision_successful = true;
                    config_file_exists = true;
                    return true;
                }
                else
                {
                    //Serial.print(SYNC); //Part of the provisioning standard
                    //Serial.print(SYNC); //Part of the provisioning standard
                    //Serial.println("Parsing of xinaboxuploader.json failed");
                    return false;
                }
            }
            else
            {
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.print(SYNC); //Part of the provisioning standard
                Serial.println("Failed to open xinaboxuploader.json");
                return false;
            }
        }
        else
        {
            SPIFFS.end();
            CREATE_XINABOX_JSON();
            return false;
        }
    }
    else
    {
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.println("Rebooting xChip. Please Flash again.");
        SPIFFS.format();
        //ESP.reset();
    }
    SPIFFS.end();
    return true;
}

bool xProvision::createConfigFile(void)
{
    return saveConfigFile();
}

bool xProvision::CREATE_XINABOX_JSON(void)
{
    //save the custom parameters to FS
    if (SPIFFS.begin())
    {
        File xinaboxFile = SPIFFS.open("/xinaboxuploader.json", "w");
        if (!xinaboxFile)
        {
            Serial.print(SYNC); //Part of the provisioning standard
            Serial.print(SYNC); //Part of the provisioning standard
            Serial.println("Failed to create config file.");
            return false;
        }
        else
        {
            DynamicJsonBuffer jsonBuffer1;
            JsonObject &json1 = jsonBuffer1.parseObject(jsonFile);

            if (json1.success())
            {
                //json1.prettyPrintTo(Serial);
                json1.printTo(xinaboxFile);
                xinaboxFile.close();
                SPIFFS.end();
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool xProvision::CHECK_XINABOX_JSON(String var1, String &var2)
{
    DynamicJsonBuffer jsonBuffer2;
    JsonObject &json2 = jsonBuffer2.parseObject(jsonFileStored);
    if (json2.success())
    {
        if (json2.containsKey(var1))
        {
            var2 = json2[var1].as<String>();
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
        return false;
    }
    return false;
}

bool xProvision::formatMemory(void)
{
    SPIFFS.format();
}