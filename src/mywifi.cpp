#include "mywifi.h"

#include <WiFi.h>
#include <Arduino.h>

bool MYWIFI::connect(const char *ssid, const char *password, int timeout) {
    WiFi.begin(ssid, password);

    long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        long elapsed = millis() - start;
        if (elapsed / 1000 > timeout) {
            return false;
        }
    }
    Serial.print("connected");
    return true;
}

void MYWIFI::disconnect() {
    WiFi.disconnect(true);
}