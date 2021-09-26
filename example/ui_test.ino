/*
 * Author: https://github.com/sepp89117/
 * Source: https://github.com/sepp89117/Teensy_UI
 * Date: 2021-09-17
 * 
 * get 565-colors from http://www.barth-dev.de/online/rgb565-color-picker/
*/

#include <XPT2046_Touchscreen.h>
#include "ILI9486_t3n.h"
#include "ili9486_t3n_font_Arial.h"
#include "ili9486_t3n_font_ArialBold.h"
#include "BUI.h" //[important] include BUI after TFT and touch libraries

//Touchscreen config and instance (Depending on your connection and TFT driver)
#define CS_PIN 7
#define TIRQ_PIN 2
XPT2046_Touchscreen ts(CS_PIN);

//TFT config
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
#define TFT_MOSI 11
#define TFT_SCLK 13
#define TFT_MISO 12
//TFT instance
ILI9486_t3n tft = ILI9486_t3n(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

//graphical user interface instance
BUI ui = BUI(&tft, &ts);

//ui buttons
void btn1_onClickHandler(); //predefine handler1 for add handler in btn1 definition
Button btn1 = Button(10, 280, 100, 30, (char *)"Settings", &btn1_onClickHandler);
Button btnMain = Button(10, 280, 100, 30, (char *)"Main");

//ui labels
Label lblTitleMain = Label(10, 10, (char *)"Main screen", Arial_32);
Label lblTitle2nd = Label(10, 10, (char *)"Settings", Arial_32);
Label lblRunningtime = Label(10, 100, (char *)"Milliseconds since the program started:", Arial_12);
Label lblMillis = Label(300, 100, (char *)"0", Arial_12);
Label lblUPS = Label(300, 10, (char *)"0", Arial_12); //updates per second

//ui checkbox
CheckBox cb1 = CheckBox(10, 100, (char *)"Show millis on main screen");
CheckBox cb2 = CheckBox(10, 125, (char *)"Enable darkmode", &cb2_onClickHandler);

//ui slider
Slider sl1 = Slider(10, 175, 200, 24, 0, 132, &sl1_onChangeHandler);

//ui NumericUpDown
NumericUpDown numUD = NumericUpDown(10, 225, 100, 32, -10, 10);

//ui BarGraph
BarGraph bg1 = BarGraph(250, 150, 40, 100, 0, 132, 0x07E0, (char*)"sl1_val");

//ui DonutGraph
DonutGraph dg1 = DonutGraph(320, 150, 0, 132, 0xFFFF, (char*)"sl1_val");

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

  //[optional] set checkbox1 font once
  cb1.setFont(Arial_14);
  
  //[optional] set background color of ui once
  //ui.setBackColor(0x033F); //default is white (0xFFFF)
  
  //[optional] set btn1 to different colors
  btn1.foreColor = 0xF000; //0xF000 = darker red
  btn1.backColor = 0xE7F1;   //0xE7F1 = light green

  //[optional] preset numericUpDowns value
  numUD.setValue(0);

  //[optional] set handler on btn3 dynamically
  btnMain.setOnClickHandler(&btnMain_onClickHandler); 

  //[optional] set barGraph1 colors yellow and red, for values >= 60 & >= 80 
  bg1.addValueColor(60, 0xFFE0); //yellow
  bg1.addValueColor(80, 0xF800); //red

  //[optional] set donutGraph1 colors white and red, for values >= 0 & >= 80 
  dg1.addValueColor(0, 0xFFFF); //white
  dg1.addValueColor(80, 0xF800); //red
  //[optional] set donutGraph1 unitName
  dg1.unitName = (char*)"X";

  //start with some screen
  getMainScreen();
}

unsigned long endUpdate = 0;
unsigned long startUpdate = 0;
char vOut [11];
char v2Out [11];

void loop()
{
  startUpdate = millis();
  
  if(cb1.checked)
  {
    ultoa(millis(), vOut, 10);
    
    lblMillis.setText(vOut);
  }
  else
  {
    lblMillis.setText((char *)"n/a");
  }

  ui.update(); //needed for ui (events and draws)

  //save last update time for benchmark updates per second
  endUpdate = millis();

  //print ups (benchmark updates per second)
  float ups = 1000.0f / (float)(endUpdate - startUpdate);
  ultoa(ups, v2Out, 10);
  lblUPS.setText(v2Out);
}

/*
 * ui screens
 */
void getMainScreen()
{
  //[important] init is necessary before adding controls to a new window
  ui.init();

  //[important] Add controls to ui
  ui.addControl(&lblTitleMain);
  ui.addControl(&lblUPS);
  ui.addControl(&lblRunningtime);
  ui.addControl(&lblMillis);
  ui.addControl(&btn1);
}

void get2ndScreen()
{
  //[important] init is necessary before adding controls to a new window
  ui.init();

  //[important] Add controls to ui
  ui.addControl(&lblTitle2nd);
  ui.addControl(&lblUPS);
  ui.addControl(&cb1);
  ui.addControl(&cb2);
  ui.addControl(&sl1);
  ui.addControl(&bg1);
  ui.addControl(&dg1);
  ui.addControl(&numUD);
  ui.addControl(&btnMain);
}

/*
 * onClickHandlers
 */
void btn1_onClickHandler()
{
  get2ndScreen();
}

void btnMain_onClickHandler()
{
  getMainScreen();
}

void cb2_onClickHandler()
{
  ui.enableDarkmode(cb2.checked);
}

void sl1_onChangeHandler()
{
  bg1.setValue(sl1.getValue());
  dg1.setValue(sl1.getValue());
}
