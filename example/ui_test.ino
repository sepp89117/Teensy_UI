//get 565-colors from http://www.barth-dev.de/online/rgb565-color-picker/

#include <XPT2046_Touchscreen.h>
#include "ILI9486_t3n.h"
#include "ili9486_t3n_font_Arial.h"
#include "ili9486_t3n_font_ArialBold.h"
#include "BUI.h" //include BUI after TFT and Touch libraries

//Touchscreen config
#define CS_PIN 7
XPT2046_Touchscreen ts(CS_PIN);
#define TIRQ_PIN 2

//TFT config
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
#define TFT_MOSI 11
#define TFT_SCLK 13
#define TFT_MISO 12
ILI9486_t3n tft = ILI9486_t3n(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

//user interface
BUI ui = BUI(&tft, &ts);

//button positions
uint16_t btnXPos1 = 10;
uint16_t btnXPos2 = 120;
uint16_t btnYPosAll = 270;

//ui buttons
void btn1_onClickHandler(); //predefine handler1 for add handler in btn1 definition
void btn2_onClickHandler(); //predefine handler2 for add handler in btn2 definition
Button btn1 = Button(btnXPos1, btnYPosAll, 100, 30, (char *)"Settings", &btn1_onClickHandler);
Button btn2 = Button(btnXPos2, btnYPosAll, 100, 30, (char *)"Info", &btn2_onClickHandler);
Button btnMain = Button(btnXPos2, btnYPosAll, 100, 30, (char *)"Main");

//ui labels
Label lblTitleMain = Label(10, 10, (char *)"Main screen", Arial_32);
Label lblTitle2nd = Label(10, 10, (char *)"Settings", Arial_32);
Label lblTitle3rd = Label(10, 10, (char *)"Info", Arial_32);
Label lblRunningtime = Label(10, 100, (char *)"Milliseconds since the program started:", Arial_12);
Label lblMillis = Label(300, 100, (char *)"0", Arial_12);
Label lblSlValue = Label(320, 156, (char *)"0", Arial_12);

//ui checkboxes
CheckBox cb1 = CheckBox(10, 100, (char *)"Show millis on main screen");

//ui slider
Slider sl1 = Slider(10, 150, 300, 24, 0, 0xFFFF, &sl1_onClickHandler);

void setup()
{
  //init Serial if needed
  //Serial.begin(115200);

  //init TFT
  tft.begin();
  tft.useFrameBuffer(1);
  tft.setRotation(3);

  //init touch
  ts.setRotation(3);
  ts.begin();

  //init UI
  btnMain.setOnClickHandler(&btnMain_onClickHandler); //[optional] set handler on btn3 dynamically
  getMainScreen();
}

void loop()
{
  if(cb1.checked)
  {
    char vOut [11];
    ultoa(millis(), vOut, 10);
    
    lblMillis.setText(vOut);
  }
  else
  {
    lblMillis.setText((char *)"n/a");
  }
  
  ui.update(); //needed for events and draws
}

/*
 * ui screens
 */
void getMainScreen()
{
  //init ui
  ui.init(); //can specify the ui background color as an argument here

  //[optional] set background color of ui
  ui.setBgColor(0xF75D); //default is white (0xFFFF)

  //set Text to UI
  ui.addControl(&lblTitleMain);
  ui.addControl(&lblRunningtime);
  ui.addControl(&lblMillis);

  //[optional] set btn2 different colors
  btn2.setTextColor(0xF000); //0xF000 = darker red
  btn2.setBgColor(0xE7F1);   //0xE7F1 = light green

  //[optional] set button positions
  btn1.setPosition(btnXPos1, btnYPosAll);
  btn2.setPosition(btnXPos2, btnYPosAll);

  //Add controls to ui
  ui.addControl(&btn1);
  ui.addControl(&btn2);
}

void get2ndScreen()
{
  //init ui
  ui.init(0xEFBD); //can specify the ui background color as an argument here

  //[optional] set button positions
  btn2.setPosition(btnXPos1, btnYPosAll);
  btnMain.setPosition(btnXPos2, btnYPosAll);

  //[optional] set checkbox1 font
  cb1.setFont(Arial_14);

  //[optional] set slider1 value
  sl1.value = 100;

  //Add controls to ui
  ui.addControl(&lblTitle2nd);
  ui.addControl(&cb1);
  ui.addControl(&sl1);
  ui.addControl(&lblSlValue);
  ui.addControl(&btn2);
  ui.addControl(&btnMain);
}

void get3rdScreen()
{
  //init ui
  ui.init(0xEF5E); //can specify the ui background color as an argument here

  //set Text to UI
  ui.addControl(&lblTitle3rd);

  //[optional] set button positions
  btn1.setPosition(btnXPos1, btnYPosAll);
  btnMain.setPosition(btnXPos2, btnYPosAll);

  //Add controls to ui
  ui.addControl(&btn1);
  ui.addControl(&btnMain);
}

/*
 * onClickHandlers
 */
void btn1_onClickHandler()
{
  get2ndScreen();
}

void btn2_onClickHandler()
{
  get3rdScreen();
}

void btnMain_onClickHandler()
{
  getMainScreen();
}

void sl1_onClickHandler()
{
  lblSlValue.setText(floatToChar(sl1.value, 1));
}


char bufFloat[10];
char* floatToChar(float value, int decis) {
  byte len = 3 + decis - 1;
  if (value >= 10) len = 4 + decis - 1;
  if (value >= 100) len = 5 + decis - 1;
  if (value >= 1000) len = 6 + decis - 1;
  if (value >= 10000) len = 7 + decis - 1;
  if (value >= 100000) len = 8 + decis - 1;
  if (value < 0) len = 4 + decis - 1;
  if (value <= -10) len = 5 + decis - 1;
  if (value <= -100) len = 6 + decis - 1;
  if (value <= -1000) len = 7 + decis - 1;

  dtostrf(value, len, decis, bufFloat);
  return bufFloat;
}
