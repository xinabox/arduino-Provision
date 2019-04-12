#include <xProvision.h>

xProvision::xProvision(void)
{
    /*nothing to construct*/
}

xProvision::~xProvision(void)
{
    /*nothing to destruct*/
}

void xProvision::begin(void)
{
    Serial.begin(BAUDSPEED);
    while (!Serial) continue;
    Serial.println();
}

void xProvision::transmit(void)
{
    //Send Provision Data
    Serial.println();
    Serial.print(SYNC);
    printConfigJson();
}

bool xProvision::recieve(void)
{   
    //Wait for data
    unsigned long currentTime = 0;
    currentTime = millis();

    while ((!Serial.available()) && (tick_prov_wait < 10)) 
    {
        if((millis() - currentTime) > 1000)
        {
            currentTime = millis();
            tick_prov_wait++;
        }
    }

    if(tick_prov_wait == 10)
    {
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.print(SYNC); //Part of the provisioning standard
        Serial.println("Provision Ended, Recieved Nothing!");
    }
    else
    {
    //Receive Provision Data
        String s = Serial.readStringUntil('\n');

        StaticJsonBuffer<512> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(s);
        if (!root.success()) {
            Serial.print(SYNC); //Part of the provisioning standard
            Serial.print(SYNC); //Part of the provisioning standard       
            Serial.println("Something went wrong!");
            return false;
        } 
        else
        {
            //root.prettyPrintTo(Serial);

            if(root.containsKey("WiFi_Network") && root.containsKey("WiFi_Password"))
            {
                _ssid = root["WiFi_Network"].as<String>();
                _pwd  = root["WiFi_Password"].as<String>();
            }

            if(root.containsKey("MQTT_Server") && root.containsKey("MQTT_Port"))
            {
                _mqtt_server = root["MQTT_Server"].as<String>();
                _mqtt_port = root["MQTT_Port"].as<String>();             
            }

            if(root.containsKey("UbiDots_Token"))
            {
                _ubidots_token  = root["UbiDots_Token"].as<String>();
            }

            if(root.containsKey("Blynk_Token"))
            {
                _blynk_token  = root["Blynk_Token"].as<String>();
            }

            if(root.containsKey("Azure_Token"))
            {
                _azure_token = root["Azure_Token"].as<String>();
            }

            if(root.containsKey("Cloud_Token"))
            {
                _cloud_token = root["Cloud_Token"].as<String>();
            }

            if(root.containsKey("LED_to_blink"))
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
        }
    }
}

void xProvision::fail(void)
{
    Serial.print(SYNC); //Part of the provisioning standard 
    Serial.print(SYNC); //Part of the provisioning standard
    Serial.print("Provision Failed.");    
}

bool xProvision::success(void)
{
    if(provision_successful)
    {
        return provision_successful;
    }
}

void xProvision::addWiFi(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["WiFi_Network"] = "Your SSID";
    root["WiFi_Password"] = "Your PSK";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getWiFi(String &var1, String &var2)
{
    if((_ssid[0] == '\0') || (_pwd[0] == '\0'))
    {
        return false;
    }
    var1 = _ssid;
    var2 = _pwd;
    return true;
}

void xProvision::addMQTT(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["MQTT_Server"] = "Server Hostname";
    root["MQTT_Port"] = "Server Port";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getMQTT(String &var1, String &var2)
{
    if((_mqtt_server[0] == '\0') || (_mqtt_port[0] == '\0'))
    {
        return false;
    }
    var1 = _mqtt_server;
    var2 = _mqtt_port;
    return true;
}

void xProvision::addUbiDotsToken(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["UbiDots_Token"] = "Long odd number";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getUbiDotsToken(String &var1)
{
    if(_ubidots_token[0] == '\0')
    {
        return false;
    }
    var1 = _ubidots_token;
    return true;
}

void xProvision::addAzureToken(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["Azure_Token"] = "Long odd number";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getAzureToken(String &var1)
{
    if(_azure_token[0] == '\0')
    {
        return false;
    }
    var1 = _azure_token;
    return true;
}

void xProvision::addBlynkToken(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["Blynk_Token"] = "Long odd number";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getBlynkToken(String &var1)
{
    if(_blynk_token[0] == '\0')
    {
        return false;
    }
    var1 = _blynk_token;
    return true;
}

void xProvision::addCloudToken(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["Cloud_Token"] = "Long odd number";

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= size; i++)
    {
        _json[i] = json[i + 1];
    }
    _json[size - 3] = '\0';
    //Serial.println(_json);
    buildConfigJson(_json);
}

bool xProvision::getCloudToken(String &var1)
{
    if(_cloud_token[0] == '\0')
    {
        return false;
    }
    var1 = _cloud_token;
    return true;
}

void xProvision::optionBlinkLED(void)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& data = root.createNestedArray("LED_to_blink");
    data.add("Red");
    data.add("Green");
    data.add("Blue");

    size_t len = root.measureLength();
    size_t size = len + 1;
    char json[size];
    char _json[size];
    root.printTo(json, size);
    for (int i = 0; i <= size; i++)
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
    for(int i = 0; i < (total + 1); i++)
    {
        prov[i] = jsonFile[i];
    }
    Serial.println(prov);
}

void xProvision::buildConfigJson(String _json)
{
    merge_json(jsonFile, _json);
    //printConfigJson();
}

void xProvision::merge_json(String obj1, String obj2)
{
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

    if (obj2[0] == '{')
    {
        for (uint16_t pos = 0; pos < len1; pos++)
        {
            obj2[pos] = obj2[pos + 1];
        }
        obj2[len1 - 2] = '\0';
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
    else if ((obj1[0] != '\"') && (obj2[0] == '\"'))
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
        Serial.println("Error in JSON");
    }
}