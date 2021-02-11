#include <Arduino.h>
#include "M5EPD_Canvas.h"
#include "M5EPD.h"
#include "M5EPD_Driver.h"
//#include "binaryttf.h"
#include <ArduinoJson.h>


#include "config.h"
#include "mywifi.h"
#include "statusbar.h"
#include "google/auth.h"
#include "google/calendar.h"

// if you need to use font.ttf define USE_SD
#define USE_SD
const char *NTP_SERVER = "ntp.nict.jp";
const long GMT_OFFSET_SEC = 60 * 60 * 9; // JST
const int DAYLIGHT_OFFSET_SEC = 0;

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
    //delay(500);
#ifdef USE_SD
    Serial.print("----- loading SD font.ttf ---------");
    esp_err_t err  = canvas.loadFont("/font.ttf", SD);
    Serial.print(err);
    if (err != ESP_OK) {
        Serial.print("font load error!!");
        return;
    }
#endif

    // 縦向きにしたので、x が 画面のheight, y が 画面のwidthになる
    canvas.createCanvas(M5EPD_PANEL_H, M5EPD_PANEL_W);
    canvas.createRender(24);
    canvas.setTextSize(24);
    //canvas.pushCanvas(0, 0, UPDATE_MODE_GLR16);

    bool connected = MYWIFI::connect(config::WIFI_SSID, config::WIFI_PASSWORD, 10);
    if (!connected) {
        canvas.drawString("wifi connect failed", 0, M5EPD_PANEL_W / 2);
        canvas.pushCanvas(0, 0, UPDATE_MODE_GLR16);
        return;
    }
    // time sync
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

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


    // calendar list
    //auto calendarList = GoogleCalendar::getCalendars(accessToken);
    //for (int i = 0 ; i < calendarList.size() ; i++) {
    //    Serial.println(calendarList[i].id);
    //}
    //// use fierst calendar id
    //String showCalendarID = calendarList[0].id;

    // get events
    struct tm start = {0};
    struct tm end = {0};
    // memo : strptime can't parse timezone.
    //        JST = UTC+9, 00:00:00 = 15:00:00 in UTC
    const char *fmt = "%Y-%m-%dT%H:%M:%S";
    //strptime("2021-02-06T15:00:00", fmt, &start);
    //strptime("2021-02-07T15:00:00", fmt, &end);
    strptime("2021-02-07T00:00:00", fmt, &start);
    strptime("2021-02-08T00:00:00", fmt, &end);
    GoogleCalendarEventList *events = GoogleCalendar::getEvents(accessToken, config::GOOGLE_CALENDAR_ID, &start, &end);

    int dy = StatusBar::height() + 10;
    for (int i = 0 ; i < events->length() ; i++) {
        auto *event = events->get(i);
        Serial.println("---- ---- ----");
        Serial.println(event->summary());
        // time
        //canvas.drawString(event->start(), 10, dy);
        //canvas.drawString(event->end(), 200 + 10, dy);
        canvas.drawString(event->startEndDateTimePeriodString(), 10, dy);
        dy += 30;
        // title
        canvas.drawString(event->summary(), 30, dy);
        dy += 35;
        canvas.drawFastHLine(0, dy, M5EPD_PANEL_H, 31);
        dy += 5;
    }
    delete events;

    //canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    // mode must be UPDATE_MODE_GLR16 when draw lines
    canvas.pushCanvas(0, 0, UPDATE_MODE_GLR16);
    MYWIFI::disconnect();
    Serial.println(" now time ");
    int utime = time(NULL);
    Serial.println(String("") + utime);
}

void loop() {
    // need delay because drawing canvas is too slow
    delay(1001);
    M5.shutdown(60 * 60);
}