/*
 * Author: https://github.com/sepp89117/
 * Source: https://github.com/sepp89117/Teensy_UI
 * Date: 2021-10-15
*/

#include "Arduino.h"
#include "Control.h"

class BUI
{
//Maximum controls per Window
#define MAXCONTROLS 20

public:
    //Touchscreen calibration for my ILI9486
    uint16_t TS_MINX = 3800;
    uint16_t TS_MINY = 190;
    uint16_t TS_MAXX = 120;
    uint16_t TS_MAXY = 3800;

    BUI(TFTLIB *tft, XPT2046_Touchscreen *ts)
    {
        _tft = tft;
        _tft->begin();

        #ifndef NOFRAMEBUFFER 
        _tft->useFrameBuffer(true);
        #endif

        _ts = ts;
        _ts->begin();
    };

    BUI(){};

    void init()
    {
        for (uint16_t i = 0; i < MAXCONTROLS; i++)
        {
            _controls[i] = new Control;
        }
    }

    void setBackColor(uint16_t bgColor)
    {
        _bgColor = bgColor;
    }

    bool addControl(Control *control)
    {
        control->enableDarkmode(darkMode);

        for (uint16_t i = 0; i < MAXCONTROLS; i++)
        {
            if (_controls[i] != nullptr)
            {
                if (_controls[i]->getType() == UNDEFINED)
                {
                    _controls[i] = control;
                    return true;
                }
            }
            else
            {
                _controls[i] = control;
                return true;
            }
        }

        return false;
    }

    void update()
    {
        _tft->fillScreen(_bgColor);

        TS_Point p;
        uint32_t now = millis();

        if (_ts->touched())
        {
            if (now - lastTouch >= touchDelay || (lastTouchedType == SLIDER && now - lastTouch >= shortTouchDelay)) //
            {
                lastTouch = millis();
                lastTouchedType = UNDEFINED;

                p = _ts->getPoint();
                p.x = map(p.x, TS_MINX, TS_MAXX, 0, _tft->width());
                p.y = map(p.y, TS_MINY, TS_MAXY, 0, _tft->height());
            }
        }

        for (uint16_t i = 0; i < MAXCONTROLS; i++)
        {
            if (_controls[i] != nullptr)
            {
                if (_controls[i]->getType() != UNDEFINED)
                {
                    if (_controls[i]->checkTouched(p.x, p.y, p.z))
                    {
                        lastTouchedType = _controls[i]->getType();

                        _controls[i]->internalOnClickHandler(p.x, p.y);

                        if (_controls[i]->clickHandler != nullptr)
                        {
                            _controls[i]->clickHandler();
                            return;
                        }
                    }

                    _controls[i]->draw(_tft);
                }
            }
        }

        #ifndef NOFRAMEBUFFER 
            _tft->updateScreen();
        #endif
    };

    void enableDarkmode(bool enable)
    {
        enable ? _bgColor = 0x0000 : _bgColor = 0xFFFF;
        darkMode = enable;

        for (uint16_t i = 0; i < MAXCONTROLS; i++)
        {
            if (_controls[i] != nullptr)
            {
                _controls[i]->enableDarkmode(darkMode);
            }
        }
    }

    void calibrateTouch()
    {        
        init();
        TS_Point p;
        delay(500);
        
        //TS_MINY
        _tft->fillScreen(0x0000);
        _tft->fillCircle(_tft->width() / 2 - 1, 0, 3, 0x07E0);
        _tft->updateScreen();
        while(!_ts->touched());
        p = _ts->getPoint();
        TS_MINY = p.y;
        delay(500);

        //TS_MAXX
        _tft->fillScreen(0x0000);
        _tft->fillCircle(_tft->width() - 1, _tft->height() / 2 - 1, 3, 0x07E0);
        _tft->updateScreen();
        while(!_ts->touched());
        p = _ts->getPoint();
        TS_MAXX = p.x;
        delay(500);

        //TS_MAXY
        _tft->fillScreen(0x0000);
        _tft->fillCircle(_tft->width() / 2 - 1, _tft->height() - 1, 3, 0x07E0);
        _tft->updateScreen();
        while(!_ts->touched());
        p = _ts->getPoint();
        TS_MAXY = p.y;
        delay(500);

        //TS_MINX
        _tft->fillScreen(0x0000);
        _tft->fillCircle(0, _tft->height() / 2 - 1, 3, 0x07E0);
        _tft->updateScreen();
        while(!_ts->touched());
        p = _ts->getPoint();
        TS_MINX = p.x;
        delay(500);

        //done
        _tft->fillScreen(0x0000);
        _tft->setCursor(0, 10);
        _tft->setFont(Arial_14);
        _tft->setTextColor(0x07E0);
        _tft->println((char*)"Calibration done!");
        _tft->println();
        _tft->print((char*)"TS_MINX: ");
        _tft->println(TS_MINX);
        _tft->print((char*)"TS_MINY: ");
        _tft->println(TS_MINY);
        _tft->print((char*)"TS_MAXX: ");
        _tft->println(TS_MAXX);
        _tft->print((char*)"TS_MAXY: ");
        _tft->println(TS_MAXY);        
        _tft->println();
        _tft->print((char*)"Touch to continue");
        _tft->updateScreen();

        while(!_ts->touched());
    }
private:
    uint16_t _bgColor = 0xFFFF;
    TFTLIB *_tft;
    XPT2046_Touchscreen *_ts;
    Control *_controls[MAXCONTROLS];
    uint32_t touchDelay = 250;
    uint32_t shortTouchDelay = 33; //33ms = check sliders touch around 30 times per second for smooth movement
    uint32_t lastTouch = 0;
    uint8_t lastTouchedType = UNDEFINED;
    bool darkMode = false;
};
