#include "telegram.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "telegram_config.h"

void telegramBegin()
{
    Serial.println("Telegram initialized");
}


// ======================================================
// URL ENCODER
// ======================================================

String urlEncode(const String &text)
{
    String encoded = "";

    const char *hex = "0123456789ABCDEF";

    for (size_t i = 0; i < text.length(); i++)
    {
        char c = text.charAt(i);

        if (
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-' ||
            c == '_' ||
            c == '.' ||
            c == '~'
        )
        {
            encoded += c;
        }
        else
        {
            encoded += '%';
            encoded += hex[(c >> 4) & 0x0F];
            encoded += hex[c & 0x0F];
        }
    }

    return encoded;
}


// ======================================================
// SEND TELEGRAM
// ======================================================

bool sendTelegramMessage(const String &message)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Telegram: WiFi tidak terhubung");
        return false;
    }

    WiFiClientSecure client;

    // Untuk prototype ESP32
    client.setInsecure();

    HTTPClient https;

    String url =
        "https://api.telegram.org/bot" +
        String(TELEGRAM_BOT_TOKEN) +
        "/sendMessage";

    Serial.println("Telegram request...");

    if (!https.begin(client, url))
    {
        Serial.println("Telegram HTTPS begin gagal");
        return false;
    }

    https.addHeader(
        "Content-Type",
        "application/x-www-form-urlencoded"
    );

    String body =
        "chat_id=" +
        String(TELEGRAM_CHAT_ID) +
        "&text=" +
        urlEncode(message);

    int httpCode = https.POST(body);

    Serial.print("Telegram HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode > 0)
    {
        String response = https.getString();

        Serial.println("Telegram Response:");
        Serial.println(response);

        https.end();

        return httpCode == 200;
    }

    Serial.print("Telegram Error: ");
    Serial.println(https.errorToString(httpCode));

    https.end();

    return false;
}