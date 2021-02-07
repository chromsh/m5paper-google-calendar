#pragma once

#include <ArduinoJson.h>

// retrieve AccessToken From RefreshToken
class GoogleAuthorization {
public:
    static DynamicJsonDocument getAccessToken(const char *refreshToken);
};