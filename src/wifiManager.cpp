#include "wifiManager.h"

#include <WiFi.h>

#include "config.h"

const char *ssid = "sbr 11 bg yg bnci i bkan u doang";
const char *password = "00000000";

unsigned long reconnectMillis = 0;

void wifiBegin()
{
    WiFi.mode(WIFI_STA);

    WiFi.setAutoReconnect(true);

    WiFi.begin(ssid,password);

    Serial.print("Connecting");

    while(WiFi.status()!=WL_CONNECTED)
    {
        Serial.print(".");
        delay(400);
    }

    Serial.println();
    Serial.println("Connected");

    Serial.print("IP : ");
    Serial.println(WiFi.localIP());

}

void wifiLoop()
{

    if(WiFi.status()==WL_CONNECTED)
        return;

    if(millis()-reconnectMillis<5000)
        return;

    reconnectMillis=millis();

    Serial.println("Reconnect WiFi");

    WiFi.disconnect();

    WiFi.begin(ssid,password);

}

bool wifiConnected()
{
    return WiFi.status()==WL_CONNECTED;
}

String getIPAddress()
{
    return WiFi.localIP().toString();
}

int wifiRSSI()
{
    return WiFi.RSSI();
}

String wifiSSID()
{
    return WiFi.SSID();
}