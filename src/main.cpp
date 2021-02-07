#include <Arduino.h>
#include "M5EPD_Canvas.h"
#include "M5EPD.h"
#include "M5EPD_Driver.h"
#include <ArduinoJson.h>


#include "secret.h"
#include "mywifi.h"
#include "statusbar.h"
#include "google/auth.h"
#include "google/calendar.h"

M5EPD_Canvas canvas(&M5.EPD);

void setup() {
    //fillRect(0, 0, 0)
    M5.begin();
    // 縦向きに変更
    M5.EPD.SetRotation(90);
    M5.TP.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    M5.BatteryADCBegin();
    delay(500);

    // 縦向きにしたので、x が 画面のheight, y が 画面のwidthになる
    canvas.createCanvas(M5EPD_PANEL_H, M5EPD_PANEL_W);
    canvas.setTextSize(3);
    canvas.drawString("jhello", int(M5EPD_PANEL_W/2), int(M5EPD_PANEL_W));
    canvas.drawString("xhello", 0, 0);
    canvas.drawString("100x100", 100, 100);
    canvas.drawString("200x200", 200, 200);
    canvas.drawString("400x200", 400, 200);
    canvas.drawString("200x400", 200, 400);
    canvas.drawString("200x600", 200, 600);
    canvas.drawString("200x800", 200, 800);
    //canvas.pushCanvas(0, 0, UPDATE_MODE_GLR16);

    bool connected = MYWIFI::connect(config::WIFI_SSID, config::WIFI_PASSWORD, 10);
    long posX = random(0, 100);
    long posY = random(0, 200);
    if (connected) {
        canvas.drawString("wifi connected", posX, posY);
    }
    else {
        canvas.drawString("wifi connected", posX, posY);
    }

    // draw status bar
    StatusBar::draw(&canvas, M5EPD_PANEL_H);
    // Google
    // memo buf[1024 * 10]をgetAccessTokenの引数に戻り値として渡すと、
    // ***ERROR*** A stack overflow in task loopTask has been detected.
    // となる。
    // char buf[1024 * 1];
    Serial.print("before auth");
    DynamicJsonDocument doc = GoogleAuthorization::getAccessToken(config::GOOGLE_REFRESH_TOKEN);
    const char *accessToken = doc["access_token"];
    Serial.println(accessToken);

    // get events
    struct tm start = {0};
    struct tm end = {0};
    const char *fmt = "%Y-%m-%dT%H:%M:%S.%z";
    strptime("2021-02-07T00:00:00+0900", fmt, &start);
    strptime("2021-02-08T00:00:00+0900", fmt, &end);
    GoogleCalendarEventList *events = GoogleCalendar::getEvents(accessToken, &start, &end);

    for (int i = 0 ; i < events->length() ; i++) {
        auto *event = events->get(i);
        Serial.println(event->summary());
    }
    delete[] events;

    //canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    // mode must be UPDATE_MODE_GLR16 when draw lines
    canvas.pushCanvas(0, 0, UPDATE_MODE_GLR16);
    MYWIFI::disconnect();
}

void loop() {
    // need delay because drawing canvas is too slow
    delay(1001);
    M5.shutdown(60 * 60);
}