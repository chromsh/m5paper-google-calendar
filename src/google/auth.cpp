#include "auth.h"
#include "http/http.h"
#include "config.h"


const char *TOKEN_ENDPOINT="https://oauth2.googleapis.com/token";

DynamicJsonDocument GoogleAuthorization::getAccessToken(const char *refreshToken) {

    MyHTTPClient client;
    KeyValues headers(5);
    //char buf[256] = {0};

    headers.add("Content-Type", "application/x-www-form-urlencoded");

    KeyValues data(5);
    data.add("client_id", config::GOOGLE_APP_CLIENT_ID);
    data.add("client_secret", config::GOOGLE_APP_CLIENT_SECRET);
    data.add("grant_type", "refresh_token");
    data.add("refresh_token", refreshToken);

    String res = client.post(TOKEN_ENDPOINT, &headers, &data);

    DynamicJsonDocument doc(1000);
    deserializeJson(doc, res);
    return doc;
}