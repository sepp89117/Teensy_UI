/*
 * Author: https://github.com/sepp89117/
 * Source: https://github.com/sepp89117/Teensy_UI
 * Date: 2021-09-17
*/

class Control
{
//define tft-libs
#if defined(_ILI9486_t3NH_)
#define TFTLIB ILI9486_t3n
#define FONTS ILI9486_t3_font_t
#elif defined(_ILI9341_t3NH_)
#define TFTLIB ILI9341_t3n
#define FONTS ILI9341_t3_font_t
#else
#error "TFT Library not supported or included after include BUI! Include BUI as last!"
#endif

//control types
#define UNDEFINED 0
#define BUTTON 1
#define LABEL 2
#define CHECKBOX 3
#define SLIDER 4
#define NUMUD 5
#define BARGRAPH 6
#define DONUTGRAPH 7

    friend class BUI;

public:
    uint16_t x = 0;
    uint16_t y = 0;
    bool enabled = true;
    uint16_t foreColor = 0x0000;
    uint16_t backColor = 0xFFDF;

    Control(){};

    void setPosition(uint16_t xPos, uint16_t yPos)
    {
        x = xPos;
        y = yPos;
    }

    void setOnClickHandler(void (*function)())
    {
        clickHandler = function;
    }

    void setFont(FONTS font)
    {
        f = font;
        fH = f.cap_height + 2;
    };

    void setText(char *text)
    {
        t = text;
    }

    uint8_t getType()
    {
        return type;
    }

protected:
    uint16_t w = 50;
    uint16_t h = 20;
    FONTS f = Arial_12;
    uint8_t fH = 14;
    char *t = (char *)"Undefined";
    bool _isTouched = false;
    void (*clickHandler)() = nullptr;
    uint8_t type = UNDEFINED;
    bool darkMode = false;

    bool checkTouched(int touchX, int touchY, int touchZ)
    {
        _isTouched = false;

        if (enabled && touchX >= x && touchX <= x + w && touchY >= y && touchY <= y + h && touchZ > 0)
        {
            _isTouched = true;
        }

        return _isTouched;
    }

    void enableDarkmode(bool enable)
    {
        darkMode = enable;

        if (darkMode)
        {
            backColor = 0x0000;
            foreColor = 0xFFDF;
        }
        else
        {
            backColor = 0xFFDF;
            foreColor = 0x0000;
        }
    }

    virtual void draw(TFTLIB *tft){

    };

    virtual void internalOnClickHandler(int touchX, int touchY) {}

    static uint16_t colorBrigthness(uint16_t color, int addBrightness)
    {
        uint8_t r, g, b;
        int test = 0;

        color565toRGB(color, r, g, b);

        test = r + addBrightness;
        if (test < 0)
            r = 0;
        else if (test > 255)
            r = 255;
        else
            r += addBrightness;

        test = g + addBrightness;
        if (test < 0)
            g = 0;
        else if (test > 255)
            g = 255;
        else
            g += addBrightness;

        test = b + addBrightness;
        if (test < 0)
            b = 0;
        else if (test > 255)
            b = 255;
        else
            b += addBrightness;

        return color565(r, g, b);
    };

    // Pass 8-bit (each) R,G,B, get back 16-bit packed color
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
    {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    //color565toRGB - converts 565 format 16 bit color to RGB
    static void color565toRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b)
    {
        r = (color >> 8) & 0x00F8;
        g = (color >> 3) & 0x00FC;
        b = (color << 3) & 0x00F8;
    }
};

class Button : public Control
{
public:
    Button()
    {
        type = BUTTON;
    };

    Button(int xPos, int yPos, int width, int height, char *text = (char *)"Button", void (*function)() = nullptr)
    {
        x = xPos;
        y = yPos;
        w = width;
        h = height;
        t = text;
        type = BUTTON;
        clickHandler = function;
    };

protected:
    void draw(TFTLIB *tft) override
    {
        if (darkMode)
        {
            myBackColor = 0x6B4D;
            myForeColor = 0xFFFF;
        }
        else
        {
            myBackColor = backColor;
            myForeColor = foreColor;
        }

        tft->setFont(f);
        int tXsize = tft->strPixelLen(t);
        int darken = 0;

        //Check if text is to long for Button
        if (tXsize + 8 > w)
        {
            bool toLong = true;
            //while text is to long, remove last char from text
            while (toLong)
            {
                t[strlen(t) - 1] = '\0';
                tXsize = tft->strPixelLen(t);
                if (tXsize + 8 <= w)
                    toLong = false;
            }
        }

        //Check if is touched, make it darker
        if (_isTouched)
            darken = -40;

        //shape of button
        //upper half background
        tft->fillRect(x + 2, y + 2, w - 4, h / 2 - 2, colorBrigthness(myBackColor, darken));
        //lower half background
        tft->fillRect(x + 2, y + h / 2, w - 4, h / 2 - 2, colorBrigthness(myBackColor, -25 + darken));
        //transition between the two halves
        tft->drawLine(x + 2, y + h / 2, x + w - 2, y + h / 2, colorBrigthness(myBackColor, -8 + darken));
        tft->drawLine(x + 2, y + 1 + h / 2, x + w - 2, y + 1 + h / 2, colorBrigthness(myBackColor, -16 + darken));
        //inner border
        tft->drawRoundRect(x + 1, y + 1, w - 2, h - 2, 2, colorBrigthness(myBackColor, 11 + darken));
        //outer border
        tft->drawRoundRect(x, y, w, h, 3, colorBrigthness(myBackColor, -55 + darken));

        //Text
        if (sizeof(t) > 1)
        {
            tft->setCursor(x + (w / 2) - (tXsize / 2), y + ((h - fH) / 2) + 1);
            if (enabled)
                tft->setTextColor(myForeColor);
            else
                tft->setTextColor(0xA534);
            tft->print(t);
        }
    }

private:
    uint16_t myForeColor = 0x0000;
    uint16_t myBackColor = 0xF79E;
};

class Label : public Control
{
public:
    Label()
    {
        type = LABEL;
    };

    Label(int xPos, int yPos, char *text = (char *)"Label", FONTS font = Arial_12)
    {
        x = xPos;
        y = yPos;
        t = text;
        type = LABEL;
        f = font;
    };

protected:
    void draw(TFTLIB *tft) override
    {
        tft->setFont(f);
        tft->setTextColor(foreColor);
        tft->setCursor(x, y);
        tft->print(t);
    }
};

class CheckBox : public Control
{
public:
    bool checked = false;

    CheckBox()
    {
        type = CHECKBOX;
    };

    CheckBox(int xPos, int yPos, char *text = (char *)"CheckBox", void (*function)() = nullptr, FONTS font = Arial_12)
    {
        x = xPos;
        y = yPos;
        t = text;
        type = CHECKBOX;
        f = font;
        w = 16;
        h = 16;
        clickHandler = function;
    };

protected:
    void draw(TFTLIB *tft) override
    {
        if (darkMode)
        {
            myBackColor = 0x6B4D;
            myForeColor = 0xFFFF;
        }
        else
        {
            myBackColor = backColor;
            myForeColor = foreColor;
        }

        tft->setFont(f);

        //the box
        tft->fillRect(x, y, 16, 16, myBackColor);
        tft->drawRect(x, y, 16, 16, myForeColor);

        //the hook
        if (checked)
        {
            tft->drawLine(x + 2, y + 7, x + 6, y + 11, myForeColor);
            tft->drawLine(x + 3, y + 7, x + 6, y + 10, myForeColor);
            tft->drawLine(x + 7, y + 9, x + 12, y + 4, myForeColor);
            tft->drawLine(x + 7, y + 10, x + 13, y + 4, myForeColor);
        }

        //the label
        tft->setTextColor(myForeColor);
        tft->setCursor(x + 22, y + ((h - fH) / 2) + 1);
        tft->print(t);
    }

    void internalOnClickHandler(int touchX, int touchY)
    {
        checked = !checked;
    }

private:
    uint16_t myForeColor = 0x0000;
    uint16_t myBackColor = 0xFFDF;
};

class Slider : public Control
{
public:
    float minValue = 0;
    float maxValue = 100;

    Slider()
    {
        type = SLIDER;
    };

    Slider(int xPos, int yPos, int width, int height, int min, int max, void (*function)() = nullptr)
    {
        x = xPos;
        y = yPos;
        w = width;
        h = height;
        minValue = min;
        maxValue = max;
        clickHandler = function;
        value = min;
        type = SLIDER;
    };

    void setValue(float Value)
    {
        if (Value <= maxValue && Value >= minValue)
            value = Value;
    }

    float getValue()
    {
        return value;
    }

protected:
    void draw(TFTLIB *tft) override
    {
        if (darkMode)
        {
            myBackColor = 0x6B4D;
        }
        else
        {
            myBackColor = backColor;
        }

        //bar + outline
        tft->fillRoundRect(x, y + h / 4, w, h / 2, h / 4, colorBrigthness(myBackColor, 0));
        tft->drawRoundRect(x, y + h / 4, w, h / 2, h / 4, colorBrigthness(myBackColor, -55));

        if (darkMode)
        {
            //sliding dot
            if (_isTouched)
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, 40));
            else
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, 60));

            //sliding dot inner border
            tft->drawCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2 - 1, colorBrigthness(myBackColor, 40));
            //sliding dot outer border
            tft->drawCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, -35));
        }
        else
        {
            //sliding dot
            if (_isTouched)
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, -21));
            else
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, -11));

            //sliding dot inner border
            tft->drawCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2 - 1, colorBrigthness(myBackColor, 11));
            //sliding dot outer border
            tft->drawCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, -55));
        }
    }

    void internalOnClickHandler(int touchX, int touchY)
    {
        float ptcX = touchX - x;
        value = (maxValue - minValue) / (float)w * ptcX;
    }

private:
    float value = 0;
    uint16_t myBackColor = 0xF79E;
};

class NumericUpDown : public Control
{
public:
    int minValue = 0;
    int maxValue = 100;

    NumericUpDown()
    {
        type = NUMUD;
    };

    NumericUpDown(int xPos, int yPos, int width, int height, int min, int max, void (*function)() = nullptr)
    {
        x = xPos;
        y = yPos;
        if (width >= 48)
            w = width;
        else
            w = 48;
        if (height >= 24)
            h = height;
        else
            h = 24;
        minValue = min;
        maxValue = max;
        clickHandler = function;
        value = min;
        t = (char *)"\0";
        type = NUMUD;
    };

    void setValue(int Value)
    {
        if (Value <= maxValue && Value >= minValue)
            value = Value;
    }

protected:
    void draw(TFTLIB *tft) override
    {
        if (darkMode)
        {
            myBackColor = 0x6B4D;
            myForeColor = 0xFFFF;
        }
        else
        {
            myBackColor = backColor;
            myForeColor = foreColor;
        }

        tft->setFont(f);
        tft->setTextColor(myForeColor);

        //background
        tft->fillRect(x, y, w, h, colorBrigthness(myBackColor, 0));
        //border
        tft->drawRect(x, y, w, h, colorBrigthness(myBackColor, -55));
        tft->drawRect(x + 1, y + 1, w - 2, h - 2, colorBrigthness(myBackColor, -20));

        //value
        tft->drawNumber(value, x + 5, y + ((h - fH) / 2) + 1);

        //up-button
        tft->fillRect(x + w - h, y, h, h / 2, colorBrigthness(myBackColor, -8));
        tft->drawRect(x + w - h, y, h, h / 2, colorBrigthness(myBackColor, -55));
        //up-arrow
        tft->fillTriangle(x + w - h / 2, y + h / 6, x + w - h / 3, y + h / 3, x + w - (float)h / 3.0f * 2.0f, y + h / 3, myForeColor);
        //down-button
        tft->fillRect(x + w - h, y + h / 2, h, h / 2, colorBrigthness(myBackColor, -8));
        tft->drawRect(x + w - h, y + h / 2, h, h / 2, colorBrigthness(myBackColor, -55));
        //down-arrow
        tft->fillTriangle(x + w - h / 2, y + (float)h / 6.0f * 5.0f, x + w - h / 3, y + (float)h / 6.0f * 4.0f, x + w - (float)h / 3.0f * 2.0f, y + (float)h / 6.0f * 4.0f, myForeColor);
    }

    void internalOnClickHandler(int touchX, int touchY)
    {
        float ptcX = touchX - x;
        float ptcY = touchY - y;

        //check if touchX on buttons
        if (ptcX >= w - h && ptcX <= w)
        {
            //check if up or down button touched
            if (ptcY <= h / 2)
            {
                //up touched
                if (value < maxValue)
                    value++;
            }
            else
            {
                //down touched
                if (value > minValue)
                    value--;
            }
        }
    }

private:
    int value = 0;
    uint16_t myForeColor = 0x0000;
    uint16_t myBackColor = 0xFFDF;
};

class BarGraph : public Control
{
public:
    struct valueColor
    {
        float value;
        uint16_t color;
        bool isSet = false;
    };
    float minValue = 0;
    float maxValue = 100;

    BarGraph()
    {
        type = BARGRAPH;
    };

    BarGraph(int xPos, int yPos, int width, int height, int min, int max, uint16_t barBaseColor, char *title, void (*function)() = nullptr)
    {
        x = xPos;
        y = yPos;
        if (width >= 40)
            w = width;
        else
            w = 40;
        if (height >= 100)
            h = height;
        else
            h = 100;
        minValue = min;
        maxValue = max;
        t = title;
        clickHandler = function;
        value = min;
        type = BARGRAPH;

        valueColors[0].value = min;
        valueColors[0].color = barBaseColor;
        valueColors[0].isSet = true;
    };

    void setValue(float Value)
    {
        if (Value <= maxValue && Value >= minValue)
            value = Value;
    }

    //Maximum 5 valueColors, but first one is set by init BarGrap (min, barBaseColor)
    bool addValueColor(float value, uint16_t color)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
            if (!valueColors[i].isSet)
            {
                valueColors[i].value = value;
                valueColors[i].color = color;
                valueColors[i].isSet = true;
                return true;
            }
        }

        return false;
    }

protected:
    void draw(TFTLIB *tft) override
    {
        if (darkMode)
        {
            myBackColor = 0x6B4D;
            myForeColor = 0xFFFF;
        }
        else
        {
            myBackColor = backColor;
            myForeColor = foreColor;
        }

        float deltaMinMax = abs(maxValue - minValue);
        float stepWidth = (h - 26.0f) / deltaMinMax;
        int divisors = 6.0f;

        tft->drawLine(x + 23, y + 23, x + 23, y + h - 5, myForeColor);   //Y-Left-Line
        tft->drawLine(x + 23, y + h - 5, x + w, y + h - 5, myForeColor); //X-Bottom-Line

        //Title
        tft->setTextColor(myForeColor);
        tft->setFont(Arial_12_Bold);
        int tXsize = tft->strPixelLen(t);
        tft->setCursor(x + w / 2 - tXsize / 2, y);
        tft->print(t);

        //Scale values and divisors
        tft->setFont(Arial_10);
        for (int i = 0; i < divisors + 1; i++)
        {
            int scaleValue = (deltaMinMax / (float)divisors * (float)i) + minValue;
            char buf[11];
            itoa(scaleValue, buf, 10);
            tXsize = tft->strPixelLen(buf);
            tft->setCursor(x + 17 - tXsize, y + h - 10 - (i * (h - 28) / (float)divisors));
            tft->print(scaleValue);

            tft->drawLine(x + 19, y + h - 5 - (i * (h - 28) / (float)divisors), x + 22, y + h - 5 - (i * (h - 28) / (float)divisors), myForeColor); //Y-Divisor
        }

        //Bar
        tft->fillRect(x + 25, y + (h - 5) + -1 * ((value - minValue) * stepWidth), w - 25, (value - minValue) * stepWidth, valueColors[0].color);
        for (uint8_t i = 1; i < 5; i++)
        {
            if (valueColors[i].isSet)
            {
                if (value >= valueColors[i].value)
                {
                    tft->fillRect(x + 25, y + (h - 5) + -1 * ((value - minValue) * stepWidth), w - 25, ((value - minValue) * stepWidth) - ((valueColors[i].value - minValue) * stepWidth), valueColors[i].color); //yellow bar
                }
            }
        }
    }

    void internalOnClickHandler(int touchX, int touchY)
    {
        
    }

private:
    float value = 0;
    uint16_t myForeColor = 0x0000;
    uint16_t myBackColor = 0xF79E;
    valueColor valueColors[5];
};

class DonutGraph : public Control
{
public:
    struct valueColor
    {
        float value;
        uint16_t color;
        bool isSet = false;
    };
    float minValue = 0;
    float maxValue = 100;
    char* unitName = (char*)"\0";

    DonutGraph()
    {
        type = DONUTGRAPH;
    };

    DonutGraph(int xPos, int yPos, int min, int max, uint16_t barBaseColor, char *title, void (*function)() = nullptr)
    {
        x = xPos;
        y = yPos;
        w = 156;
        h = 120;
        minValue = min;
        maxValue = max;
        t = title;
        clickHandler = function;
        value = min;
        type = DONUTGRAPH;

        valueColors[0].value = min;
        valueColors[0].color = barBaseColor;
        valueColors[0].isSet = true;
    };

    void setValue(float Value)
    {
        if (Value <= maxValue && Value >= minValue)
            value = Value;
    }

    //Maximum 5 valueColors, but first one is set by init BarGrap (min, barBaseColor)
    bool addValueColor(float value, uint16_t color)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
            if (!valueColors[i].isSet)
            {
                valueColors[i].value = value;
                valueColors[i].color = color;
                valueColors[i].isSet = true;
                return true;
            }
        }

        return false;
    }

protected:
    void draw(TFTLIB *tft) override
    {
        if (darkMode)
        {
            myBackColor = 0x2945;
            myForeColor = 0xFFFF;
        }
        else
        {
            myBackColor = 0x4228;
            myForeColor = 0xFFFF;
        }

        int r = 65;
        int centerX = x + w / 2;
        int centerY = y + 82;
        float deltaVal = abs(maxValue - minValue);

        //background
        tft->fillRoundRect(x, y, w, h, 4, myBackColor);

        //border right b+60
        tft->drawLine(x + w - 2, y + 1, x + w - 2, y + h - 2, colorBrigthness(myBackColor, 60));
        tft->drawLine(x + w - 1, y + 3, x + w - 1, y + h - 5, colorBrigthness(myBackColor, 60));
        tft->drawPixel(x + w - 3, y + 2, colorBrigthness(myBackColor, 95));
        //border bottom b+35
        tft->drawLine(x + 3, y + h - 1, x + w - 4, y + h - 1, colorBrigthness(myBackColor, 35));
        tft->drawLine(x + 1, y + h - 2, x + w - 2, y + h - 2, colorBrigthness(myBackColor, 35));
        tft->drawPixel(x + w - 3, y + h - 3, colorBrigthness(myBackColor, 48));
        //border left b+135
        tft->drawLine(x, y + 3, x, y + h - 5, colorBrigthness(myBackColor, 135));
        tft->drawLine(x + 1, y + 1, x + 1, y + h - 2, colorBrigthness(myBackColor, 135));
        tft->drawPixel(x + 2, y + h - 3, colorBrigthness(myBackColor, 85));
        //border top b+180
        tft->drawLine(x + 3, y, x + w - 4, y, colorBrigthness(myBackColor, 180));
        tft->drawLine(x + 1, y + 1, x + w - 2, y + 1, colorBrigthness(myBackColor, 180));
        tft->drawPixel(x + 2, y + 2, colorBrigthness(myBackColor, 158));

        //half circle fill black background       
        float stepW = 0.4;
        int x1, y1, x2, y2, x3, y3, x4, y4;

        for (float i = 0.0f; i < 180.0f; i += stepW)
        {
            getCircleCoord(centerX, centerY, r - 16, -180.0f + i, x1, y1);
            getCircleCoord(centerX, centerY, r - 16, -180.0f + i + 1, x4, y4);
            getCircleCoord(centerX, centerY, r - 1, -180.0f + i, x2, y2);
            getCircleCoord(centerX, centerY, r - 1, -180.0f + i + 1, x3, y3);

            tft->fillTriangle(x1, y1, x2, y2, x3, y3, 0x0000);
            tft->fillTriangle(x1, y1, x2, y2, x4, y4, 0x0000);
        }

        //outer half circle
        tft->drawCircleHelper(centerX, centerY, r, 1, colorBrigthness(myBackColor, 70));
        tft->drawCircleHelper(centerX, centerY, r, 2, colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX - r, centerY,colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX + r, centerY,colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX, centerY-r,colorBrigthness(myBackColor, 70));
        tft->drawCircleHelper(centerX, centerY, r - 1, 1, colorBrigthness(myBackColor, 100));
        tft->drawCircleHelper(centerX, centerY, r - 1, 2, colorBrigthness(myBackColor, 100));
        tft->drawPixel(centerX - r+1, centerY,colorBrigthness(myBackColor, 100));
        tft->drawPixel(centerX + r-1, centerY,colorBrigthness(myBackColor, 100));
        tft->drawPixel(centerX, centerY-r+1,colorBrigthness(myBackColor, 100));

        //inner half circle
        tft->drawCircleHelper(centerX, centerY, r - 16, 1, colorBrigthness(myBackColor, 70));
        tft->drawCircleHelper(centerX, centerY, r - 16, 2, colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX - r+16, centerY,colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX + r-16, centerY,colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX, centerY-r+16,colorBrigthness(myBackColor, 70));
        tft->drawCircleHelper(centerX, centerY, r - 17, 1, colorBrigthness(myBackColor, 40));
        tft->drawCircleHelper(centerX, centerY, r - 17, 2, colorBrigthness(myBackColor, 40));
        tft->drawPixel(centerX - r+17, centerY,colorBrigthness(myBackColor, 40));
        tft->drawPixel(centerX + r-17, centerY,colorBrigthness(myBackColor, 40));
        tft->drawPixel(centerX, centerY-r+17,colorBrigthness(myBackColor, 40));

        for (int i = 0; i < 9; i++)
        {
            float angle = -180.0f / 8.0f * (float)i;
            getCircleCoord(centerX, centerY, r - 5, angle, x1, y1);
            getCircleCoord(centerX, centerY, r - 13, angle, x2, y2);

            tft->drawLine(x1, y1, x2, y2, 0xFFFF);
        }

        uint16_t drawinColor = valueColors[0].color;
        float valueAngle = 180.0f / deltaVal * value;
        for (float i = 0.0f; i < valueAngle; i += stepW)
        {
            getCircleCoord(centerX, centerY, r - 27, -180.0f + i, x1, y1);
            getCircleCoord(centerX, centerY, r - 27, -180.0f + i + 1, x4, y4);
            getCircleCoord(centerX, centerY, r - 16, -180.0f + i, x2, y2);
            getCircleCoord(centerX, centerY, r - 16, -180.0f + i + 1, x3, y3);

            //get color to draw            
            for (uint8_t c = 1; c < 5; c++)
            {
                if (valueColors[c].isSet)
                {
                    float colorAngle = 180.0f / deltaVal * valueColors[c].value;

                    if (i >= colorAngle) {
                        drawinColor = valueColors[c].color;
                    }
                }
            }

            tft->fillTriangle(x1, y1, x2, y2, x3, y3, drawinColor);
            tft->fillTriangle(x1, y1, x2, y2, x4, y4, drawinColor);
        }

        //print segment values
        float valSegment = deltaVal / 8.0f;
        int tXsize;
        float segval;
        bool multipl = false;
        tft->setTextColor(myForeColor);
        tft->setFont(Arial_11);
        char str[14];
        //segval 8
        segval = maxValue;
        ltoa(segval, str, 10);
        if (segval > 1000) segval /= 100.0f, multipl = true;
        tXsize = tft->strPixelLen(str);
        tft->setCursor(x + 141 - tXsize / 2, y + 87);
        tft->print(segval, 0);
        //segval 6
        tft->setCursor(x + 125, y + 25);
        segval = valSegment * 6.0f + (float)minValue;
        ltoa(segval, str, 10);
        if (multipl) segval /= 100.0f;
        tft->print(segval, 0);
        //segval 4
        segval = valSegment * 4.0f + (float)minValue;
        ltoa(segval, str, 10);
        if (multipl) segval /= 100.0f;
        tXsize = tft->strPixelLen(str);
        tft->setCursor(centerX - tXsize / 2, y + 5);
        tft->print(segval, 0);
        //segval 2
        segval = valSegment * 2.0f + (float)minValue;
        ltoa(segval, str, 10);
        if (multipl) segval /= 100.0f;
        tXsize = tft->strPixelLen(str);
        tft->setCursor(x + 33 - tXsize, y + 25);
        tft->print(segval, 0);
        //segval 0
        segval = minValue;
        ltoa(segval, str, 10);
        if (multipl && segval != 0) segval /= 100.0f;
        tXsize = tft->strPixelLen(str);
        tft->setCursor(x + 16 - tXsize / 2, y + 87);
        tft->print(segval, 0);
        //print multiplier
        if (multipl) {
            tft->setCursor(x + 119, y + 5);
            tft->print("x100");
        }


        //print value
        ltoa(value, str, 10);
        tft->setFont(Arial_20);
        int tXPos = centerX - tft->strPixelLen(str) / 2;
        tft->setTextColor(drawinColor);
        tft->setCursor(tXPos, centerY-Arial_20.cap_height/2);
        tft->print(str);

        //print text
        tft->setTextColor(myForeColor);
        tft->setFont(Arial_12_Bold);
        tXPos = centerX - tft->strPixelLen(t) / 2;
        tft->setCursor(tXPos, centerY+17);
        tft->print(t);

        //print unit name
        tft->setTextColor(myForeColor);
        tft->setFont(Arial_12_Bold);
        tXPos = centerX - tft->strPixelLen(unitName) / 2;
        tft->setCursor(tXPos, centerY-28);
        tft->print(unitName);



        //graph
        // tft->fillRect(x + 25, y + (h - 5) + -1 * ((value - minValue) * stepWidth), w - 25, (value - minValue) * stepWidth, valueColors[0].color);
        // for (uint8_t i = 1; i < 5; i++)
        // {
        //     if (valueColors[i].isSet)
        //     {
        //         if (value >= valueColors[i].value) {
        //             tft->fillRect(x + 25, y + (h - 5) + -1 * ((value - minValue) * stepWidth), w - 25, ((value - minValue) * stepWidth) - ((valueColors[i].value - minValue) * stepWidth), valueColors[i].color); //yellow bar
        //         }
        //     }
        // }
    }

    void internalOnClickHandler(int touchX, int touchY)
    {
        
    }

private:
    float value = 0;
    uint16_t myForeColor = 0x0000;
    uint16_t myBackColor = 0xF79E;
    valueColor valueColors[5];

    void getCircleCoord(int centerX, int centerY, int radius, float angle, int &outX, int &outY)
    {
        outX = (float)centerX + cos((angle * PI) / 180.0f) * (float)radius;
        outY = (float)centerY + sin((angle * PI) / 180.0f) * (float)radius;
    }
};