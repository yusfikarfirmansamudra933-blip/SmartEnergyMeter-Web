#include "wifiManager.h"

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

namespace {
constexpr unsigned long RECONNECT_INTERVAL_MS = 5000;
unsigned long reconnectMillis = 0;
constexpr unsigned long RESTART_DELAY_MS = 1500;

AsyncWebServer setupServer(80);
DNSServer dnsServer;
Preferences wifiPreferences;
String storedSSID;
String storedPassword;
String setupNetwork;
String setupPassword;
bool provisioning = false;
bool restartPending = false;
unsigned long restartAt = 0;

const char SETUP_PAGE[] PROGMEM = R"HTML(
<!doctype html><html lang="id"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Smart Energy Meter setup</title><body><h1>Hubungkan Wi-Fi</h1>
<p>Masukkan jaringan Wi-Fi rumah atau kantor Anda.</p>
<form method="post" action="/save"><label>Nama Wi-Fi<br><input name="ssid" required></label><br><br>
<label>Password Wi-Fi<br><input name="password" type="password"></label><br><br>
<button type="submit">Simpan dan hubungkan</button></form></body></html>
)HTML";

String deviceSuffix()
{
    char suffix[7];
    snprintf(suffix, sizeof(suffix), "%06X", static_cast<uint32_t>(ESP.getEfuseMac() & 0xFFFFFF));
    return String(suffix);
}

void startProvisioning()
{
    const String suffix = deviceSuffix();
    setupNetwork = "SmartMeter-" + suffix;
    setupPassword = "SEM-" + suffix;

    WiFi.mode(WIFI_AP);
    WiFi.softAP(setupNetwork.c_str(), setupPassword.c_str());
    dnsServer.start(53, "*", WiFi.softAPIP());

    setupServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", SETUP_PAGE);
    });
    setupServer.on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (!request->hasParam("ssid", true))
        {
            request->send(400, "text/plain", "Wi-Fi name is required");
            return;
        }

        const String ssid = request->getParam("ssid", true)->value();
        const String password = request->hasParam("password", true)
            ? request->getParam("password", true)->value()
            : "";

        if (ssid.length() == 0)
        {
            request->send(400, "text/plain", "Wi-Fi name is required");
            return;
        }

        wifiPreferences.putString("ssid", ssid);
        wifiPreferences.putString("password", password);
        restartPending = true;
        restartAt = millis() + RESTART_DELAY_MS;
        request->send(200, "text/html", "<h1>Saved</h1><p>Device is restarting and will join your Wi-Fi.</p>");
    });
    setupServer.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });
    setupServer.begin();
    provisioning = true;

    Serial.printf("Wi-Fi setup: %s / %s\n", setupNetwork.c_str(), setupPassword.c_str());
}
}

void wifiBegin()
{
    wifiPreferences.begin("wifi", false);
    storedSSID = wifiPreferences.getString("ssid", "");
    storedPassword = wifiPreferences.getString("password", "");

    if (storedSSID.length() == 0)
    {
        startProvisioning();
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(storedSSID.c_str(), storedPassword.c_str());
    Serial.println("Connecting to WiFi");

}

void wifiLoop()
{

    if (provisioning)
    {
        dnsServer.processNextRequest();
        if (restartPending && millis() >= restartAt)
            ESP.restart();
        return;
    }

    if(WiFi.status()==WL_CONNECTED)
        return;

    if (millis() - reconnectMillis < RECONNECT_INTERVAL_MS)
        return;

    reconnectMillis=millis();

    Serial.println("Reconnect WiFi");

    WiFi.disconnect();

    WiFi.begin(storedSSID.c_str(), storedPassword.c_str());

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

bool wifiProvisioning()
{
    return provisioning;
}

String wifiSetupNetwork()
{
    return setupNetwork;
}

String wifiSetupPassword()
{
    return setupPassword;
}

void resetWiFiCredentials()
{
    wifiPreferences.clear();
}
