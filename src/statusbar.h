#pragma once

#include "M5EPD_Canvas.h"

class StatusBar {
public:
    // draw statusbar
    static void draw(M5EPD_Canvas *canvas, long width);
};