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
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, 0));
            else
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, 20));
            
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