#include "PowerLimit.h"
#include "globals.h"
#include "telegram.h"


static bool telegramLimitSent = false;


void checkPowerLimit()
{
    // ==================================================
    // POWER PERCENT
    // ==================================================

    if (powerLimit > 0)
    {
        powerPercent =
            (int)((power / powerLimit) * 100.0);


        if (powerPercent < 0)
            powerPercent = 0;


        if (powerPercent > 100)
            powerPercent = 100;
    }
    else
    {
        powerPercent = 0;
    }


    // ==================================================
    // LIMIT TERCAPAI
    // ==================================================

    if (
        powerLimit > 0 &&
        power >= powerLimit
    )
    {
        overload = true;

        overloadCount++;


        // ==================================================
        // TELEGRAM HANYA SEKALI
        // ==================================================

        if (!telegramLimitSent)
        {
            String message;


            message +=
                "🚨 SMART ENERGY METER\n\n";

            message +=
                "⚠️ POWER LIMIT TERCAPAI\n\n";


            message +=
                "Daya sekarang : ";

            message +=
                String(power, 1);

            message +=
                " W\n";


            message +=
                "Batas daya    : ";

            message +=
                String(powerLimit, 1);

            message +=
                " W\n";


            message +=
                "Tegangan      : ";

            message +=
                String(voltage, 1);

            message +=
                " V\n";


            message +=
                "Arus          : ";

            message +=
                String(current, 2);

            message +=
                " A\n";


            message +=
                "Frekuensi     : ";

            message +=
                String(frequency, 2);

            message +=
                " Hz\n";


            message +=
                "Power Factor  : ";

            message +=
                String(pf, 2);

            message +=
                "\n\n";


            message +=
                "Status: ⚠️ LIMIT";


            if(
                sendTelegramMessage(
                    message
                )
            )
            {
                telegramLimitSent =
                    true;

                Serial.println(
                    "Telegram LIMIT terkirim"
                );
            }
            else
            {
                Serial.println(
                    "Telegram LIMIT gagal"
                );
            }
        }
    }


    // ==================================================
    // KEMBALI NORMAL
    // ==================================================

    else
    {
        overload = false;


        if(telegramLimitSent)
        {
            String message;


            message +=
                "✅ SMART ENERGY METER\n\n";

            message +=
                "POWER KEMBALI NORMAL\n\n";


            message +=
                "Daya sekarang : ";

            message +=
                String(power, 1);

            message +=
                " W\n";


            message +=
                "Batas daya    : ";

            message +=
                String(powerLimit, 1);

            message +=
                " W\n";


            message +=
                "Status: ✅ NORMAL";


            sendTelegramMessage(
                message
            );
        }


        telegramLimitSent =
            false;
    }
}