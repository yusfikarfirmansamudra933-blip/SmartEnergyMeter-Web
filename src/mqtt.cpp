#include "mqtt.h"
#include "storage.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "globals.h"

//==========================================================
// MQTT CLIENT
//==========================================================

WiFiClientSecure espClient;
PubSubClient mqtt(espClient);

//==========================================================
// EMQX CLOUD
//==========================================================

const char* MQTT_HOST = "l660c516.ala.eu-central-1.emqxsl.com";
const int MQTT_PORT = 8883;

const char* MQTT_USER = "smartenergymeter";
const char* MQTT_PASS = "12345678";

//==========================================================
// MQTT TOPIC
//==========================================================

const char* TOPIC_DATA    = "smartmeter/data";
const char* TOPIC_LIMIT   = "smartmeter/cmd/limit";
const char* TOPIC_RESTART = "smartmeter/cmd/restart";
const char* TOPIC_RESET   = "smartmeter/cmd/reset";

//==========================================================
// MQTT CALLBACK
//==========================================================

void callback(char* topic, byte* payload, unsigned int length)
{
    String message;

    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    Serial.print("Topic : ");
    Serial.println(topic);

    Serial.print("Message : ");
    Serial.println(message);

    //======================================================
    // POWER LIMIT
    //======================================================

    if (String(topic) == TOPIC_LIMIT)
    {
        float value = message.toFloat();

        if (value >= 100 && value <= 10000)
        {
            powerLimit = value;

            saveConfig();

            Serial.print("Limit Baru : ");
            Serial.println(powerLimit);
        }
        else
        {
            Serial.println("Limit tidak valid");
        }
    }

    //======================================================
    // RESTART ESP32
    //======================================================

    else if (String(topic) == TOPIC_RESTART)
    {
        Serial.println("===== MQTT RESTART =====");

        delay(500);

        ESP.restart();
    }

    //======================================================
    // FACTORY RESET
    //======================================================

    else if (String(topic) == TOPIC_RESET)
    {
        Serial.println("===== MQTT FACTORY RESET =====");

        resetConfig();

        delay(1000);

        ESP.restart();
    }
}

//==========================================================
// MQTT BEGIN
//==========================================================

void mqttBegin()
{
    // Menggunakan TLS tanpa verifikasi sertifikat
    espClient.setInsecure();

    mqtt.setServer(MQTT_HOST, MQTT_PORT);

    mqtt.setCallback(callback);

    Serial.println("MQTT initialized");
}

//==========================================================
// MQTT RECONNECT
//==========================================================

void mqttReconnect()
{
    while (!mqtt.connected())
    {
        Serial.println("========================");
        Serial.println("Connecting to MQTT...");

        if (mqtt.connect(
            "ESP32SmartMeter",
            MQTT_USER,
            MQTT_PASS
        ))
        {
            Serial.println("MQTT CONNECTED");

            // Subscribe hanya ke command yang diperlukan
            mqtt.subscribe(TOPIC_LIMIT);
            mqtt.subscribe(TOPIC_RESTART);
            mqtt.subscribe(TOPIC_RESET);

            Serial.println("MQTT SUBSCRIBED");
        }
        else
        {
            Serial.print("FAILED");
            Serial.print("  rc=");
            Serial.println(mqtt.state());

            delay(5000);
        }
    }
}

//==========================================================
// MQTT LOOP
//==========================================================

void mqttLoop()
{
    if (!mqtt.connected())
    {
        mqttReconnect();
    }

    mqtt.loop();
}

//==========================================================
// MQTT PUBLISH
//==========================================================

void mqttPublish()
{
    if (!mqtt.connected())
    {
        Serial.println("MQTT NOT CONNECTED");
        return;
    }

    StaticJsonDocument<512> doc;

    // Data PZEM
    doc["voltage"] = voltage;
    doc["current"] = current;
    doc["power"] = power;
    doc["energy"] = energy;
    doc["frequency"] = frequency;
    doc["pf"] = pf;

    // Daya
    doc["va"] = apparentPower;
    doc["var"] = reactivePower;

    // Power limiter
    doc["limit"] = powerLimit;

    // Status
    doc["wifi"] = (WiFi.status() == WL_CONNECTED);
    doc["sensor"] = sensorOnline;
    doc["trip"] = overload;

    String json;

    serializeJson(doc, json);

    Serial.print("Publish : ");
    Serial.println(json);

    if (mqtt.publish(TOPIC_DATA, json.c_str()))
    {
        Serial.println("PUBLISH SUCCESS");
    }
    else
    {
        Serial.println("PUBLISH FAILED");
    }
}