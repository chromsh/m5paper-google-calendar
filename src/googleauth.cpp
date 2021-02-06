#include "googleauth.h"
#include <HTTPClient.h>


const char *TOKEN_ENDPOINT="https://oauth2.googleapis.com/token";

void GoogleAuthorization::getAccessToken(const char *refreshToken, char *out) {
    HTTPClient http;

    // TODO ここから
}