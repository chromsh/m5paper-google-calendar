#include "statusbar.h"

#include "M5EPD.h"

void StatusBar::draw(M5EPD_Canvas *canvas, long width) {

    // bar
    canvas->drawFastHLine(0, 32, width, 27);


    // battery percentage calculation from
    // https://github.com/m5stack/M5Paper_FactoryTest/blob/ef8d1ff94490a9364479231d6ba7e343d9adaa06/src/frame/frame_main.cpp#L272
    uint32_t vol = M5.getBatteryVoltage();

    if(vol < 3300)
    {
        vol = 3300;
    }
    else if(vol > 4350)
    {
        vol = 4350;
    }
    float battery = (float)(vol - 3300) / (float)(4350 - 3300);
    if(battery <= 0.01)
    {
        battery = 0.01;
    }
    if(battery > 1)
    {
        battery = 1;
    }

    int margin_top = 5;
    char battString[32];
    sprintf(battString, "batt: %.1f %%", battery * 100);
    canvas->drawString(battString, width - 140, margin_top);

    // time string
    struct tm now;
    bool succeeded = getLocalTime(&now);
    if (!succeeded) {
        Serial.println("getLocalTime() failed");
    }
    char buf[30];
    strftime(buf, sizeof(buf)-1, "%Y-%m-%d %H:%M:%S", &now);
    canvas->drawString(buf, 5, margin_top);
}
