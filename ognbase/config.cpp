/*
   CONFIG.cpp
   Copyright (C) 2020 Manuel Rösel

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "SoftRF.h"
#include "EEPROM.h"
#include "global.h"
#include "config.h"
#include <ArduinoJson.h>

//wifi
String ogn_ssid[5];
String ogn_wpass[5];
int    ssid_index = 0;

//aprs default values
String   ogn_callsign    = "callsign";
String   ogn_server      = "aprs.glidernet.org";
uint16_t ogn_port        = 14580;
uint8_t  ogn_band        = 1;
uint8_t  ogn_protocol_1  = 0;
uint8_t  ogn_protocol_2  = 1;
bool     ogn_debug       = false;
uint16_t ogn_debugport   = 12000;
bool     ogn_itrackbit   = false;
bool     ogn_istealthbit = false;
bool     ogn_sleepmode   = false;
uint16_t ogn_rxidle      = 3600;
uint16_t ogn_wakeuptimer = 3600;
uint8_t  ogn_range       = 100;

//position
float   ogn_lat              = 0;
float   ogn_lon              = 0;
int     ogn_alt              = 0;
uint8_t ogn_geoid_separation = 0;
uint8_t largest_range        = 0;

//fanet service
bool fanet_enable = false;

//zabbix
bool   zabbix_enable = false;
String zabbix_server = "127.0.0.1";
int    zabbix_port   = 10051;
String zabbix_key    = "ogn_base";

//supporters
bool beers_show = false;


bool OGN_read_config(void)
{
    const size_t        capacity = 1024;
    DynamicJsonDocument baseConfig(capacity);
    JsonObject          obj;

    const char* kerror;

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return false;
    }


    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println(F("Failed to open config.json."));
        return false;
    }

    DeserializationError error = deserializeJson(baseConfig, configFile);

    if (error)
    {
        Serial.println(F("Failed to read file, using default configuration"));
        configFile.close();
        return false;
    }
    else
    {
        obj = baseConfig.as<JsonObject>();
        configFile.close();
    }

    if (!obj.containsKey(F("wifi")))
        return false;
    else
    {
        Serial.println(F("found wifi config!"));
        if (1)
            for (int i=0; i < 5; i++) {
                ogn_ssid[i]  = obj["wifi"]["ssid"][i].as<String>();
                ogn_wpass[i] = obj["wifi"]["pass"][i].as<String>();
            }
    }

    if (!obj.containsKey(F("coordinates")))
        return false;
    else
    {
        Serial.println(F("found coordinates config!"));
        if (1)
        {
            ogn_lat              = obj["coordinates"]["lat"];
            ogn_lon              = obj["coordinates"]["lon"];
            ogn_alt              = obj["coordinates"]["alt"];
            ogn_geoid_separation = obj["coordinates"]["geoidsep"];
        }
    }

    if (!obj.containsKey(F("aprs")))
        return false;
    else
    {
        Serial.println(F("found aprs config!"));
        if (1)
        {
            ogn_callsign = obj["aprs"]["callsign"].as<String>();
            ogn_server   = obj["aprs"]["server"].as<String>();
            ogn_port     = obj["aprs"]["port"];

            ogn_band        = obj["aprs"]["band"];
            ogn_protocol_1  = obj["aprs"]["protocol_1"];
            ogn_protocol_2  = obj["aprs"]["protocol_2"];
            ogn_debug       = obj["aprs"]["debug"];
            ogn_debugport   = obj["aprs"]["debugport"];
            ogn_itrackbit   = obj["aprs"]["itrackbit"];
            ogn_istealthbit = obj["aprs"]["istealthbit"];
            ogn_sleepmode   = obj["aprs"]["sleepmode"];
            ogn_rxidle      = obj["aprs"]["rxidle"];
            ogn_wakeuptimer = obj["aprs"]["wakeuptimer"];
        }
    }


    if (!obj.containsKey(F("zabbix")))
        return false;
    else
    {
        Serial.println(F("found zabbix config!"));
        if (1)
        {
            zabbix_enable = obj["zabbix"]["enable"];
            zabbix_server = obj["zabbix"]["server"].as<String>();
            ;
            zabbix_port = obj["zabbix"]["port"];
            zabbix_key  = obj["zabbix"]["key"].as<String>();
            ;
        }
    }

    if (!obj.containsKey(F("fanetservice")))
        return false;
    else
    {
        Serial.println(F("found fanetservice config!"));
        if (1)
            fanet_enable = obj["fanetservice"]["enable"];
    }

    if (!obj.containsKey(F("beers")))
    {}
    else if (1)
        beers_show = obj["beers"]["show"];



    return true;
}

bool OGN_save_config(void)
{
    const size_t        capacity = 1024;
    DynamicJsonDocument baseConfig(capacity);
    JsonObject          obj;

    const char* kerror;

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return false;
    }


    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println(F("Failed to open config.json readonly"));
        return false;
    }

    DeserializationError error = deserializeJson(baseConfig, configFile);

    if (error)
    {
        Serial.println(F("Failed to read file, using default configuration"));
        configFile.close();
        return false;
    }
    else
    {
        obj = baseConfig.as<JsonObject>();
        configFile.close();
    }

    configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
        Serial.println(F("Failed to open config.json write operation"));
        return false;
    }

    //position config
    obj["coordinates"]["lat"]      = ogn_lat;
    obj["coordinates"]["lon"]      = ogn_lon;
    obj["coordinates"]["alt"]      = ogn_alt;
    obj["coordinates"]["geoidsep"] = ogn_geoid_separation;
    //aps config
    obj["aprs"]["callsign"] = ogn_callsign;

    obj["aprs"]["server"] = ogn_server;
    obj["aprs"]["port"]   = ogn_port;

    obj["aprs"]["band"]        = ogn_band;
    obj["aprs"]["protocol_1"]  = ogn_protocol_1;
    obj["aprs"]["protocol_2"]  = ogn_protocol_2;
    obj["aprs"]["debug"]       = ogn_debug;
    obj["aprs"]["debugport"]   = ogn_debugport;
    obj["aprs"]["itrackbit"]   = ogn_itrackbit;
    obj["aprs"]["istealthbit"] = ogn_istealthbit;
    obj["aprs"]["sleepmode"]   = ogn_sleepmode;
    obj["aprs"]["rxidle"]      = ogn_rxidle;
    obj["aprs"]["wakeuptimer"] = ogn_wakeuptimer;

    //wifi config
    obj["wifi"]["ssid"][0] =  ogn_ssid[0];
    obj["wifi"]["pass"][0] =  ogn_wpass[0];
    //fanet config
    obj["fanetservice"]["enable"] = fanet_enable;
    //zabbix config
    obj["zabbix"]["enable"] = zabbix_enable;
    obj["zabbix"]["server"] = zabbix_server;
    obj["zabbix"]["port"]   = zabbix_port;
    obj["zabbix"]["key"]    = zabbix_key;

    if (serializeJson(obj, configFile) == 0)
        Serial.println(F("Failed to write to file"));

    configFile.close();
    return true;
}
