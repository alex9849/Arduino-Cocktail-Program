#include "main.h"
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x5AAA

#define BUTTON_W 40
#define BUTTON_H 24
#define START_BUTTON_W 120
#define START_BUTTON_H 70
#define START_BUTTON_PADDING_DOWN 30

#define MINPRESSURE 5
#define MAXPRESSURE 1000

#define TS_MINX 80
#define TS_MAXX 910

#define TS_MINY 120
#define TS_MAXY 910

#define MAIN_MOTOR_INTERFACE 1
#define MAIN_MOTOR_DIR 50
#define MAIN_MOTOR_STEP 68

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
int zutatenAnzahl{5};
Elegoo_GFX_Button plusButtons[5] = {};
Elegoo_GFX_Button minusButtons[5] = {};
Elegoo_GFX_Button startButton{};
int zutaten[5] = {};
unsigned long lastScreenTouch;
unsigned long touchScreenTimeDelta = 40;
bool cocktailActive = false;
const int ELEMENT_COUNT = 20;
Array<MotorCommand, ELEMENT_COUNT> motorCommands;
AccelStepper mainMotor(MAIN_MOTOR_INTERFACE, MAIN_MOTOR_STEP, MAIN_MOTOR_DIR);



void setup() {
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
   
  }
  tft.begin(identifier);
  tft.setRotation(3);
  initButtons();
  updateGui();
  mainMotor.setMaxSpeed(3000);
	mainMotor.setAcceleration(200);
	mainMotor.setSpeed(200);
  mainMotor.moveTo(2000);
  /*Serial.println(motorCommands.size());
  motorCommands.push_back(MotorCommand{&mainMotor, int{2000}});
  Serial.println(motorCommands.size());*/
}

void loop() {
  // Move the motor one step


  /*if(!motorCommands.empty()) {
    if(motorCommands.back().done()) {
      motorCommands.pop_back();
    } else {
      motorCommands.back().executeTick();
    }
  }*/

  // put your main code here, to run repeatedly:
  processGui();
}

void processGui() {
  TSPoint p = ts.getPoint();
  int16_t tmp = p.x;
  p.x = p.y;
  p.y = tmp;
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if(p.z > MINPRESSURE && p.z < MAXPRESSURE && !cocktailActive && (millis() - lastScreenTouch > touchScreenTimeDelta)) {
    lastScreenTouch = millis();
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

    for(int i{0}; i < zutatenAnzahl; ++i) {
      processZutatenButton(plusButtons[i], p, zutaten[i], true);
      processZutatenButton(minusButtons[i], p, zutaten[i], false);
    }

    //Start Button
    if(startButton.contains(p.x, p.y)) {
      startCocktail();
    }
  }
}

void startCocktail() {
  
  cocktailActive = true;
  initButtons();

  for(int zutatIndex{0}; zutatIndex < zutatenAnzahl; ++zutatIndex) {
    mainMotor.moveTo(zutatIndex * 1000);
    Serial.println(zutaten[zutatIndex]);
    while (mainMotor.distanceToGo() != 0 && zutaten[zutatIndex] != 0) {
      mainMotor.run();
    }

    for(int dipNr{0}; dipNr < zutaten[zutatIndex]; ++dipNr) {
      
    }
  }
}

void processZutatenButton(Elegoo_GFX_Button &button, TSPoint &p, int &zutatenZahl, bool isPlus) {
  if(button.contains(p.x, p.y)) {
    if(isPlus) {
      zutatenZahl = zutatenZahl + 2;
      updateGui();
    } else {
      if(zutatenZahl != 0) {
        zutatenZahl = zutatenZahl - 2;
        updateGui(); 
      }
    }
  }
}


void initButtons() {
  uint16_t plusButtonColor = cocktailActive? GRAY:GREEN;
  uint16_t minusButtonColor = cocktailActive? GRAY:RED;
  uint16_t startButtonColor = cocktailActive? GRAY:GREEN;
  for(int i{0}; i < zutatenAnzahl; ++i) {
    plusButtons[i].initButton(&tft, tft.width() - (BUTTON_W / 2), BUTTON_H * i + (BUTTON_H / 2), 
          BUTTON_W, BUTTON_H, 
          1, plusButtonColor, WHITE,
          "+", 2); 
    plusButtons[i].drawButton();
    minusButtons[i].initButton(&tft, tft.width() - (BUTTON_W + BUTTON_W / 2), BUTTON_H * i + (BUTTON_H / 2), 
          BUTTON_W, BUTTON_H, 
          1, minusButtonColor, WHITE,
          "-", 2); 
    minusButtons[i].drawButton();
  }

  startButton.initButton(&tft, tft.width() / 2, tft.height() - START_BUTTON_H / 2 - START_BUTTON_PADDING_DOWN, 
          START_BUTTON_W, START_BUTTON_H, 
          1, startButtonColor, WHITE,
          "Start", 2); 
  startButton.drawButton();
}
void updateGui() {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  for(int i{0}; i < zutatenAnzahl; ++i) {
    String line = "Zutat ";
    line += (i + 1);
    line += ": ";
    line += zutaten[i];
    line += "cl";
    tft.println(line);
  }
  for(int i{0}; i < zutatenAnzahl; ++i) {
    plusButtons[i].drawButton();
    minusButtons[i].drawButton();
  }
  startButton.drawButton();
}