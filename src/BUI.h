#include "Arduino.h"
#include "Control.h"

class BUI
{
//Touchscreen calibration
#define TS_MINX 3800
#define TS_MINY 190
#define TS_MAXX 120
#define TS_MAXY 3800

//Maximum controls per Window
#define MAXCONTROLS 20

public:
    BUI(TFTLIB *tft, XPT2046_Touchscreen *ts)
    {
        _tft = tft;
        _ts = ts;
    };

    BUI(){};

    void init(uint16_t bgColor = 0xFFFF)
    {
        for (uint16_t i = 0; i < MAXCONTROLS; i++)
        {
            _controls[i] = new Control;
        }

        _bgColor = bgColor;
    }

    void setBgColor(uint16_t bgColor)
    {
        _bgColor = bgColor;
    }

    void addControl(Control *control)
    {
        for (uint16_t i = 0; i < MAXCONTROLS; i++)
        {
            if (_controls[i] != nullptr)
            {
                if (_controls[i]->getType() == UNDEFINED)
                {
                    _controls[i] = control;
                    break;
                }
            }
            else
            {
                _controls[i] = control;
                break;
            }
        }
    }

    void update()
    {
        _tft->fillScreen(_bgColor);

        TS_Point p;
        unsigned long now = millis();
        bool oneIsTouched = false;

        if (_ts->touched() && now - lastTouch >= touchDelay)
        {
            lastTouch = millis();
            p = _ts->getPoint();
            p.x = map(p.x, TS_MINX, TS_MAXX, 0, _tft->width());
            p.y = map(p.y, TS_MINY, TS_MAXY, 0, _tft->height());
        }

        for (uint16_t i = 0; i < MAXCONTROLS; i++)
        {
            if (_controls[i] != nullptr)
            {
                if (_controls[i]->getType() != UNDEFINED)
                {
                    if(!oneIsTouched) oneIsTouched = _controls[i]->checkTouched(p.x, p.y, p.z);
                    else break;
                    _controls[i]->draw(_tft);
                }
            }
        }

        if(!oneIsTouched) _tft->updateScreen();
    };

private:
    uint16_t _bgColor = 0xFFFF;
    TFTLIB *_tft;
    XPT2046_Touchscreen *_ts;
    Control *_controls[MAXCONTROLS];
    uint16_t touchDelay = 250;
    unsigned long lastTouch = 0;
};