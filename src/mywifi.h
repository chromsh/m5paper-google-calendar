#pragma once


#include <WiFi.h>
#include <Arduino.h>

// wifi utility
class MYWIFI {
public:
    static bool connect(const char *ssid, const char *password, int timeout);
    static void disconnect();
};
