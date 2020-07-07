#pragma once

#include <Arduino.h>
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

void setup();
void loop();
void processZutatenButton(Elegoo_GFX_Button &button, TSPoint &p, int &zutatenZahl, bool isPlus);
void initButtons();
void updateGui();