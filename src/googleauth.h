#pragma once

// retrieve AccessToken From RefreshToken
class GoogleAuthorization {
public:
    static void getAccessToken(const char *refreshToken);
};