#include "pzem.h"

#include <Arduino.h>
#include <PZEM004Tv30.h>

#include "config.h"
#include "globals.h"

HardwareSerial PZEMSerial(2);

PZEM004Tv30 pzem(PZEMSerial, RXD2, TXD2);

//======================================================

void pzemBegin()
{
    PZEMSerial.begin(
        9600,
        SERIAL_8N1,
        RXD2,
        TXD2
    );
}

//======================================================

void readPZEM()
{
    float v = pzem.voltage();
    float i = pzem.current();
    float p = pzem.power();
    float e = pzem.energy();
    float f = pzem.frequency();
    float pfValue = pzem.pf();

    // -----------------------------
    // Validasi Sensor
    // -----------------------------

    if(isnan(v))
    {
        sensorOnline = false;
        return;
    }

    sensorOnline = true;

    voltage = v;
    current = i;
    power = p;
    energy = e;
    frequency = f;
    pf = pfValue;

    // -----------------------------
    // Hitung Apparent Power (VA)
    // -----------------------------

    apparentPower = voltage * current;

    // -----------------------------
    // Hitung Reactive Power (VAR)
    // -----------------------------

    float s = apparentPower * apparentPower;
    float w = power * power;

    float q = s - w;

    if(q < 0)
        q = 0;

    reactivePower = sqrt(q);

    // -----------------------------
    // Filter nilai aneh
    // -----------------------------

    if(voltage < 0) voltage = 0;
    if(current < 0) current = 0;
    if(power < 0) power = 0;
    if(energy < 0) energy = 0;
    if(frequency < 0) frequency = 0;
    if(pf < 0) pf = 0;

    // PF maksimum 1.0
    if(pf > 1)
        pf = 1;

}