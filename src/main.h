#pragma once

#include <Arduino.h>
#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
#include <TouchScreen.h>
#include <AccelStepper.h>

void setup();
void loop();
void processZutatenButton(Elegoo_GFX_Button &button, TSPoint &p, int &zutatenZahl, bool isPlus);
void initButtons();
void updateGui();
void makeCocktail();
void processGui();
void referenceRun();