/*
    Bondar libraries
    Copyright (C) 2017  Associació Cosmic Research

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

#ifndef COSMIC_IRIDIUM
#define COSMIC_IRIDIUM

#include "Senscape.h"

class IridiumSBD {

private:

    Serial* serial;
    Countdown* countdown;

    uint8_t sleepPin;
    uint8_t ringPin;

    uint32_t baudRate;

    bool powered;

    static void ((*onStartDone)(error_t));
    static void ((*onStopDone)(error_t));
    static void ((*onSendDone)(error_t));
    static void ((*onReceiveDone)(const uint8_t*, size_t, error_t));

    void send(const char* text);
    void send(uint16_t number);

public:

    IridiumSBD(Serial* serial, uint8_t sleepPin, uint8_t ringPin, uint32_t baudRate = 19200);

    error_t start();
    error_t stop();
    error_t sendText(const char* text);
    error_t sendBinary(const uint8_t* txData, size_t size);
    error_t sendReceiveText(const char* txText);
    error_t sendReceiveBinary(const uint8_t* txData, size_t txSize);
    bool isAsleep();
    error_t sleep();
    void powerOn();
    void powerOff();

    bool waitForATResponse();

    void attachStartDone(void (*)(error_t));
    void attachStopDone(void (*)(error_t));
    void attachSendDone(void (*)(error_t));
    void attachReceiveDone(void (*)(const uint8_t*, size_t, error_t));
};

#endif