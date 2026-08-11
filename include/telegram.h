#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <Arduino.h>

void telegramBegin();

bool sendTelegramMessage(const String &message);

#endif