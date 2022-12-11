/*
 * Author: https://github.com/sepp89117/
 * Source: https://github.com/sepp89117/Teensy_UI
 * Date: 2022-05-19
 */

// define tft-libs
#if defined(_ILI9486_t3NH_)
#define TFTLIB ILI9486_t3n
#define FONTS ILI9486_t3_font_t

#elif defined(_ILI9341_t3NH_)
#define TFTLIB ILI9341_t3n
#define FONTS ILI9341_t3_font_t

#elif defined(__SSD1351_t3_H_)
#define TFTLIB SSD1351_t3
#define FONTS GFXfont

#elif defined(__ST7735_t3_H_)
#define TFTLIB ST7735_t3
#define FONTS GFXfont

#elif defined(_ILI9488_t3H_)
#define TFTLIB ILI9488_t3
#define FONTS ILI9341_t3_font_t

#elif defined(_ILI9486_esp32_)
#define TFTLIB ILI9486_esp32
#define FONTS ILI9341_esp32_font_t

#else
#error "TFT Library not supported or included after include BUI! Include BUI as last!"
#endif

// control types
#define UNDEFINED 0
#define BUTTON 1
#define LABEL 2
#define CHECKBOX 3
#define SLIDER 4
#define NUMUD 5
#define BARGRAPH 6
#define DONUTGRAPH 7
#define IMAGE 8
#define DROPDOWN 9
#define TEXTBOX 10
#define NUMPAD 11
#define BOX 12

// aligns
#define ALIGNLEFT 0
#define ALIGNCENTER 1
#define ALIGNRIGHT 2
#define ALIGNTOP 3
#define ALIGNMIDDLE 4
#define ALIGNBOTTOM 5

class Control
{
    friend class BUI;

public:
    uint16_t x = 0;
    uint16_t y = 0;
    bool enabled = true;
    uint16_t foreColor = 0x0000;
    uint16_t backColor = 0xFFDF;
    bool _isTouched = false;

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

    void setText(const String &text)
    {
        t = (char *)text.c_str();
    }

    void setText(long text)
    {
        ltoa(text, str, 10);
        t = str;
    }

    void setText(float text, uint8_t strLength, uint8_t decimalPlaces)
    {
        dtostrf(text, strLength, decimalPlaces, str);
        t = str;
    }

    uint8_t getType()
    {
        return type;
    }

protected:
    uint16_t w = 50;
    uint16_t h = 20;
    FONTS f = Arial_14;
    uint8_t fH = 14;
    char *t = (char *)"Undefined";
    void (*clickHandler)() = nullptr;
    uint8_t type = UNDEFINED;
    bool darkMode = false;
    bool priorized = false;
    char str[100];

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

    virtual void draw(TFTLIB *tft){}

    virtual void internalOnClickHandler(int touchX, int touchY) {}

    virtual void ddClickHandler(int touchX, int touchY) {}

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

    // color565toRGB - converts 565 format 16 bit color to RGB
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

    void setImage(uint16_t width, uint16_t height, const uint16_t *data)
    {
        _image = data;
        _imageW = width;
        _imageH = height;
    }

    void removeImage()
    {
        _image = NULL;
    }

    uint8_t style = 1;

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

        // Check if text is to long for Button
        if (tXsize + 8 + (_image != NULL ? (_imageW / 2) : 0) > w)
        {
            bool toLong = true;
            // while text is to long, remove last char from text
            while (toLong)
            {
                t[strlen(t) - 1] = '\0';
                tXsize = tft->strPixelLen(t);
                if (tXsize + 8 + (_image != NULL ? (_imageW / 2) : 0) <= w)
                    toLong = false;
            }
        }

        
        if(style == 0) {
            // buttons shape
            // upper half background
            tft->fillRect(x + 2, y + 2, w - 4, h / 2 - 2, colorBrigthness(myBackColor, darken));
            // lower half background
            tft->fillRect(x + 2, y + h / 2, w - 4, h / 2 - 2, colorBrigthness(myBackColor, -25 + darken));
            // transition between the two halves
            tft->drawLine(x + 2, y + h / 2, x + w - 2, y + h / 2, colorBrigthness(myBackColor, -8 + darken));
            tft->drawLine(x + 2, y + 1 + h / 2, x + w - 2, y + 1 + h / 2, colorBrigthness(myBackColor, -16 + darken));
            // inner border
            tft->drawRoundRect(x + 1, y + 1, w - 2, h - 2, 2, colorBrigthness(myBackColor, 11 + darken));
            // outer border
            tft->drawRoundRect(x, y, w, h, 3, colorBrigthness(myBackColor, -55 + darken));
        } else if(style == 1){
            // outer border
            tft->drawRoundRect(x, y, w, h, 4, 0xEF7D);
            tft->drawRoundRect(x+1, y+1, w-2, h-2, 4, 0xEF7D);

            //infill
            tft->fillRect(x + 2, y + 2, w - 4, h - 4, 0xEF7D);
        } else {
            // outer border
            tft->drawRoundRect(x, y, w, h, 3, colorBrigthness(myBackColor, -55 + darken));
        }

        // Image
        if (_image != NULL)
        {
            for (uint16_t imgY = 0; imgY < _imageH; imgY++)
            {
                if (imgY < h - 4) // make shure we are in V bounds of button (hide V image overflow)
                    for (uint16_t imgX = 0; imgX < _imageW; imgX++)
                    {
                        if (imgX < w - 4) // make shure we are in H bounds of button (hide H image overflow)
                        {
                            tft->drawPixel(x + 2 + imgX, y + 2 + imgY, _image[imgY * _imageW + imgX]);
                        }
                        else
                            break;
                    }
                else
                    break;
            }
        }

        // Text
        if (sizeof(t) > 1)
        {
            tft->setCursor(x + (w / 2) - (tXsize / 2) + (_image != NULL ? (_imageW / 2) : 0), y + ((h - fH) / 2) + 1);

            if(style == 0) {
                if (enabled)
                    tft->setTextColor(myForeColor);
                else
                    tft->setTextColor(0xA534);
            } else if(style == 1){
                if (enabled)
                    tft->setTextColor(0x0000);
                else
                    tft->setTextColor(0xA534);
            } else {
                if (enabled)
                    tft->setTextColor(myForeColor);
                else
                    tft->setTextColor(0xA534);
            }
            tft->print(t);
        }
    }

private:
    uint16_t myForeColor = 0x0000;
    uint16_t myBackColor = 0xF79E;
    const uint16_t *_image = NULL;
    uint16_t _imageW;
    uint16_t _imageH;
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

    uint8_t textAlign = ALIGNLEFT; // horizontal text alignment

protected:
    void draw(TFTLIB *tft) override
    {
        tft->setFont(f);
        tft->setTextColor(foreColor);

        uint16_t textWidth = tft->strPixelLen(t);
        switch (textAlign)
        {
        case ALIGNLEFT:
            tft->setCursor(x, y);
            break;
        case ALIGNCENTER:
            tft->setCursor(x - textWidth / 2, y);
            break;
        case ALIGNRIGHT:
            tft->setCursor(x - textWidth, y);
            break;
        default:
            tft->setCursor(x, y);
        }

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

        // the box
        tft->fillRect(x, y, 16, 16, myBackColor);
        tft->drawRect(x, y, 16, 16, myForeColor);

        // the hook
        if (checked)
        {
            tft->drawLine(x + 2, y + 7, x + 6, y + 11, myForeColor);
            tft->drawLine(x + 3, y + 7, x + 6, y + 10, myForeColor);
            tft->drawLine(x + 7, y + 9, x + 12, y + 4, myForeColor);
            tft->drawLine(x + 7, y + 10, x + 13, y + 4, myForeColor);
        }

        // the label
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

        // bar + outline
        tft->fillRoundRect(x, y + h / 4, w, h / 2, h / 4, colorBrigthness(myBackColor, 0));
        tft->drawRoundRect(x, y + h / 4, w, h / 2, h / 4, colorBrigthness(myBackColor, -55));

        if (darkMode)
        {
            // sliding dot
            if (_isTouched)
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, 40));
            else
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, 60));

            // sliding dot inner border
            tft->drawCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2 - 1, colorBrigthness(myBackColor, 40));
            // sliding dot outer border
            tft->drawCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, -35));
        }
        else
        {
            // sliding dot
            if (_isTouched)
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, -21));
            else
                tft->fillCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2, colorBrigthness(myBackColor, -11));

            // sliding dot inner border
            tft->drawCircle(((w - h / 2) / (maxValue - minValue) * value) + h / 2 + x, y + h / 2, h / 2 - 1, colorBrigthness(myBackColor, 11));
            // sliding dot outer border
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
    float step = 1;
    int decimalPlaces = 0;

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
            myForeColor = 0xFFFF;
        }
        else
        {
            myBackColor = backColor;
            myForeColor = foreColor;
        }

        tft->setFont(f);
        tft->setTextColor(myForeColor);

        // background
        tft->fillRect(x, y, w, h, colorBrigthness(myBackColor, 0));
        // border
        tft->drawRect(x, y, w, h, colorBrigthness(myBackColor, -55));
        tft->drawRect(x + 1, y + 1, w - 2, h - 2, colorBrigthness(myBackColor, -20));

        // value
        dtostrf(value, 3, decimalPlaces, valueText);
        tft->setCursor(x + h / 2 - (f.cap_height + 2) / 2, y + ((h - fH) / 2) + 1);
        tft->print(valueText);
        // tft->drawString(valueText, x + 5, y + ((h - fH) / 2) + 1);

        // up-button
        tft->fillRect(x + w - h, y, h, h / 2, colorBrigthness(myBackColor, -8));
        tft->drawRect(x + w - h, y, h, h / 2, colorBrigthness(myBackColor, -55));
        // up-arrow
        tft->fillTriangle(x + w - h / 2, y + h / 6, x + w - h / 3, y + h / 3, x + w - (float)h / 3.0f * 2.0f, y + h / 3, myForeColor);
        // down-button
        tft->fillRect(x + w - h, y + h / 2, h, h / 2, colorBrigthness(myBackColor, -8));
        tft->drawRect(x + w - h, y + h / 2, h, h / 2, colorBrigthness(myBackColor, -55));
        // down-arrow
        tft->fillTriangle(x + w - h / 2, y + (float)h / 6.0f * 5.0f, x + w - h / 3, y + (float)h / 6.0f * 4.0f, x + w - (float)h / 3.0f * 2.0f, y + (float)h / 6.0f * 4.0f, myForeColor);
    }

    void internalOnClickHandler(int touchX, int touchY)
    {
        float ptcX = touchX - x;
        float ptcY = touchY - y;

        // check if touchX on buttons
        if (ptcX >= w - h && ptcX <= w)
        {
            // check if up or down button touched
            if (ptcY <= h / 2)
            {
                // up touched
                if (value < maxValue)
                {
                    value += step;
                    if (value > maxValue)
                        value = maxValue;
                }
            }
            else
            {
                // down touched
                if (value > minValue)
                {
                    value -= step;
                    if (value < minValue)
                        value = minValue;
                }
            }
        }
    }

private:
    float value = 0;
    char valueText[7];
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
        else if (Value > maxValue)
            value = maxValue;
        else if (Value < minValue)
            value = minValue;
    }

    // Maximum 5 valueColors, but first one is set by init BarGrap (min, barBaseColor)
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
        float stepWidth = ((float)h - 26.0f) / deltaMinMax;
        int divisors = 4;

        tft->drawLine(x + 19, y + 23, x + 21, y + 23, myForeColor);   // left-top-border
        tft->drawLine(x + 21, y + 23, x + 21, y + h - 5, myForeColor);   // Y-Left-Line
        tft->drawLine(x + 21, y + h - 5, x + w, y + h - 5, myForeColor); // X-Bottom-Line
        tft->drawLine(x + w, y + 23, x + w, y + h - 5, myForeColor);   // Y-Right-Line
        tft->drawLine(x + w, y + 23, x + w + 2, y + 23, myForeColor);   // right-top-border

        // Title
        tft->setTextColor(myForeColor);
        tft->setFont(Arial_12_Bold);
        int tXsize = tft->strPixelLen(t);
        tft->setCursor(x + w / 2 - tXsize / 2, y);
        tft->print(t);

        // Bar
        uint16_t barX = x + 23;
        uint16_t barY = y + (h - 5) + (-1.0f * ((value - minValue) * stepWidth)) - 2;
        uint16_t barW = w - 24;
        uint16_t barH = (value - minValue) * stepWidth - 3;

        //bg color
        tft->fillRect(x + 22, y + 23, w-22, h-28, 0x39C7); //dark gray
        //first color
        tft->fillRect(barX, barY, barW, barH, valueColors[0].color);
        for (uint8_t i = 1; i < 5; i++)
        {
            if (valueColors[i].isSet)
            {
                if (value >= valueColors[i].value)
                {
                    barY = y + (h - 5) + -1 * ((value - minValue) * stepWidth);                                // 200 + -110 = 90
                    barH = ((value - minValue) * stepWidth) - ((valueColors[i].value - minValue) * stepWidth); // 110

                    tft->fillRect(barX, barY, barW, barH, valueColors[i].color); // yellow bar
                }
            }
        }
        
        // Scale values and divisors
        tft->setFont(Arial_10);
        for (int i = 1; i < divisors + 1; i++)
        {
            int scaleValue = (deltaMinMax / (float)divisors * (float)i) + minValue;
            char buf[11];
            itoa(scaleValue, buf, 10);
            tXsize = tft->strPixelLen(buf);
            tft->setCursor(x + 17 - tXsize, y + h - 10 - (i * (h - 28) / (float)divisors));
            tft->print(scaleValue);

            tft->drawLine(x + 22, y + h - 5 - (i * (h - 28) / (float)divisors), x + 30, y + h - 5 - (i * (h - 28) / (float)divisors), 0x325F); // Y-Divisor
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
    char *unitName = (char *)"\0";

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

    // Maximum 5 valueColors, but first one is set by init BarGrap (min, barBaseColor)
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

        // background
        tft->fillRoundRect(x, y, w, h, 4, myBackColor);

        // border right b+60
        tft->drawLine(x + w - 2, y + 1, x + w - 2, y + h - 2, colorBrigthness(myBackColor, 60));
        tft->drawLine(x + w - 1, y + 3, x + w - 1, y + h - 5, colorBrigthness(myBackColor, 60));
        tft->drawPixel(x + w - 3, y + 2, colorBrigthness(myBackColor, 95));
        // border bottom b+35
        tft->drawLine(x + 3, y + h - 1, x + w - 4, y + h - 1, colorBrigthness(myBackColor, 35));
        tft->drawLine(x + 1, y + h - 2, x + w - 2, y + h - 2, colorBrigthness(myBackColor, 35));
        tft->drawPixel(x + w - 3, y + h - 3, colorBrigthness(myBackColor, 48));
        // border left b+135
        tft->drawLine(x, y + 3, x, y + h - 5, colorBrigthness(myBackColor, 135));
        tft->drawLine(x + 1, y + 1, x + 1, y + h - 2, colorBrigthness(myBackColor, 135));
        tft->drawPixel(x + 2, y + h - 3, colorBrigthness(myBackColor, 85));
        // border top b+180
        tft->drawLine(x + 3, y, x + w - 4, y, colorBrigthness(myBackColor, 180));
        tft->drawLine(x + 1, y + 1, x + w - 2, y + 1, colorBrigthness(myBackColor, 180));
        tft->drawPixel(x + 2, y + 2, colorBrigthness(myBackColor, 158));

        // half circle fill black background
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

        // outer half circle
        tft->drawCircleHelper(centerX, centerY, r, 1, colorBrigthness(myBackColor, 70));
        tft->drawCircleHelper(centerX, centerY, r, 2, colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX - r, centerY, colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX + r, centerY, colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX, centerY - r, colorBrigthness(myBackColor, 70));
        tft->drawCircleHelper(centerX, centerY, r - 1, 1, colorBrigthness(myBackColor, 100));
        tft->drawCircleHelper(centerX, centerY, r - 1, 2, colorBrigthness(myBackColor, 100));
        tft->drawPixel(centerX - r + 1, centerY, colorBrigthness(myBackColor, 100));
        tft->drawPixel(centerX + r - 1, centerY, colorBrigthness(myBackColor, 100));
        tft->drawPixel(centerX, centerY - r + 1, colorBrigthness(myBackColor, 100));

        // inner half circle
        tft->drawCircleHelper(centerX, centerY, r - 16, 1, colorBrigthness(myBackColor, 70));
        tft->drawCircleHelper(centerX, centerY, r - 16, 2, colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX - r + 16, centerY, colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX + r - 16, centerY, colorBrigthness(myBackColor, 70));
        tft->drawPixel(centerX, centerY - r + 16, colorBrigthness(myBackColor, 70));
        tft->drawCircleHelper(centerX, centerY, r - 17, 1, colorBrigthness(myBackColor, 40));
        tft->drawCircleHelper(centerX, centerY, r - 17, 2, colorBrigthness(myBackColor, 40));
        tft->drawPixel(centerX - r + 17, centerY, colorBrigthness(myBackColor, 40));
        tft->drawPixel(centerX + r - 17, centerY, colorBrigthness(myBackColor, 40));
        tft->drawPixel(centerX, centerY - r + 17, colorBrigthness(myBackColor, 40));

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

            // get color to draw
            for (uint8_t c = 1; c < 5; c++)
            {
                if (valueColors[c].isSet)
                {
                    float colorAngle = 180.0f / deltaVal * valueColors[c].value;

                    if (i >= colorAngle)
                    {
                        drawinColor = valueColors[c].color;
                    }
                }
            }

            tft->fillTriangle(x1, y1, x2, y2, x3, y3, drawinColor);
            tft->fillTriangle(x1, y1, x2, y2, x4, y4, drawinColor);
        }

        // print segment values
        float valSegment = deltaVal / 8.0f;
        int tXsize;
        float segval;
        bool multipl = false;
        tft->setTextColor(myForeColor);
        tft->setFont(Arial_11);
        char str[14];
        // segval 8
        segval = maxValue;
        ltoa(segval, str, 10);
        if (segval > 1000)
            segval /= 100.0f, multipl = true;
        tXsize = tft->strPixelLen(str);
        tft->setCursor(x + 141 - tXsize / 2, y + 87);
        tft->print(segval, 0);
        // segval 6
        tft->setCursor(x + 125, y + 25);
        segval = valSegment * 6.0f + (float)minValue;
        ltoa(segval, str, 10);
        if (multipl)
            segval /= 100.0f;
        tft->print(segval, 0);
        // segval 4
        segval = valSegment * 4.0f + (float)minValue;
        ltoa(segval, str, 10);
        if (multipl)
            segval /= 100.0f;
        tXsize = tft->strPixelLen(str);
        tft->setCursor(centerX - tXsize / 2, y + 5);
        tft->print(segval, 0);
        // segval 2
        segval = valSegment * 2.0f + (float)minValue;
        ltoa(segval, str, 10);
        if (multipl)
            segval /= 100.0f;
        tXsize = tft->strPixelLen(str);
        tft->setCursor(x + 33 - tXsize, y + 25);
        tft->print(segval, 0);
        // segval 0
        segval = minValue;
        ltoa(segval, str, 10);
        if (multipl && segval != 0)
            segval /= 100.0f;
        tXsize = tft->strPixelLen(str);
        tft->setCursor(x + 16 - tXsize / 2, y + 87);
        tft->print(segval, 0);
        // print multiplier
        if (multipl)
        {
            tft->setCursor(x + 119, y + 5);
            tft->print("x100");
        }

        // print value
        ltoa(value, str, 10);
        tft->setFont(Arial_20);
        int tXPos = centerX - tft->strPixelLen(str) / 2;
        tft->setTextColor(drawinColor);
        tft->setCursor(tXPos, centerY - Arial_20.cap_height / 2);
        tft->print(str);

        // print text
        tft->setTextColor(myForeColor);
        tft->setFont(Arial_12_Bold);
        tXPos = centerX - tft->strPixelLen(t) / 2;
        tft->setCursor(tXPos, centerY + 17);
        tft->print(t);

        // print unit name
        tft->setTextColor(myForeColor);
        tft->setFont(Arial_12_Bold);
        tXPos = centerX - tft->strPixelLen(unitName) / 2;
        tft->setCursor(tXPos, centerY - 28);
        tft->print(unitName);

        // graph
        //  tft->fillRect(x + 25, y + (h - 5) + -1 * ((value - minValue) * stepWidth), w - 25, (value - minValue) * stepWidth, valueColors[0].color);
        //  for (uint8_t i = 1; i < 5; i++)
        //  {
        //      if (valueColors[i].isSet)
        //      {
        //          if (value >= valueColors[i].value) {
        //              tft->fillRect(x + 25, y + (h - 5) + -1 * ((value - minValue) * stepWidth), w - 25, ((value - minValue) * stepWidth) - ((valueColors[i].value - minValue) * stepWidth), valueColors[i].color); //yellow bar
        //          }
        //      }
        //  }
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

class Image : public Control
{
public:
    Image()
    {
        type = IMAGE;
    };

    Image(int xPos, int yPos, int width, int height, const uint16_t *data)
    {
        x = xPos;
        y = yPos;
        w = width;
        h = height;
        _data = data;
        type = IMAGE;
    };

    uint8_t imageHAlign = ALIGNLEFT; // horizontal image alignment
    uint8_t imageVAlign = ALIGNTOP;  // vertical image alignment
    bool visible = true;
    const uint16_t *_data = NULL;
    uint8_t scale = 1;

protected:
    void draw(TFTLIB *tft) override
    {
        uint16_t _x = x;
        uint16_t _y = y;

        switch (imageHAlign)
        {
        case ALIGNLEFT:
            _x = x;
            break;
        case ALIGNCENTER:
            _x = x - w / 2;
            break;
        case ALIGNRIGHT:
            _x = x - w;
            break;
        default:
            _x = x;
        }

        switch (imageVAlign)
        {
        case ALIGNTOP:
            _y = y;
            break;
        case ALIGNMIDDLE:
            _y = y - h / 2;
            break;
        case ALIGNBOTTOM:
            _y = y - h;
            break;
        default:
            _y = y;
        }

        if (visible)
        {
            if (scale == 1)
                tft->writeRect(_x, _y, w, h, _data);
            else
            {
                for (uint16_t sY = 0; sY < h * scale; sY++)
                {
                    for (uint16_t sX = 0; sX < w * scale; sX++)
                    {
                        uint16_t index = sY / scale * w + sX / scale;
                        tft->drawPixel(sX, sY, _data[index]);
                    }
                }
            }
        }
    }

private:
};

class Box : public Control
{
public:
    Box()
    {
        type = BOX;
    };

    Box(int xPos, int yPos, int width, int height, uint16_t color)
    {
        x = xPos;
        y = yPos;
        w = width;
        h = height;
        myBackColor = color;
        type = BOX;
    };

    uint8_t imageHAlign = ALIGNLEFT; // horizontal image alignment
    uint8_t imageVAlign = ALIGNTOP;  // vertical image alignment
    bool visible = true;

protected:
    void draw(TFTLIB *tft) override
    {        
        uint16_t _x = x;
        uint16_t _y = y;

        switch (imageHAlign)
        {
        case ALIGNLEFT:
            _x = x;
            break;
        case ALIGNCENTER:
            _x = x - w / 2;
            break;
        case ALIGNRIGHT:
            _x = x - w;
            break;
        default:
            _x = x;
        }

        switch (imageVAlign)
        {
        case ALIGNTOP:
            _y = y;
            break;
        case ALIGNMIDDLE:
            _y = y - h / 2;
            break;
        case ALIGNBOTTOM:
            _y = y - h;
            break;
        default:
            _y = y;
        }

        if (visible)
        {
            tft->fillRect(_x, _y, w, h, myBackColor);
        }
    }

private:
    uint16_t myBackColor = 0xFFDF;
};

class DropDown : public Control
{
public:
    DropDown()
    {
        type = DROPDOWN;
    };

    DropDown(int xPos, int yPos, int width, int height, void (*function)() = nullptr)
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
        clickHandler = function;
        t = (char *)"\0";
        type = DROPDOWN;
    };

    bool addOption(char *text)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if (options[i] == NULL || options[i][0] == '\0')
            {
                options[i] = text;
                _optionsCount += 1;
                return true;
            }
        }

        return false;
    }

    char *getSelectedOption()
    {
        return options[_selectedIndex];
    }

    int8_t getSelectedIndex()
    {
        return _selectedIndex;
    }

    bool setSelectedIndex(int8_t index)
    {
        if (index == -1)
        {
            _selectedIndex = index;
            t = '\0';
            return true;
        }

        if (options[index] != NULL && options[index][0] != '\0')
        {
            _selectedIndex = index;
            t = options[index];
            return true;
        }
        else
            return false;
    }

    uint8_t getOptionsCount()
    {
        return _optionsCount;
    }

protected:
    uint16_t ddWidth = 0;
    uint16_t ddHeight = 0;
    uint16_t ddX = 0;
    uint16_t ddY = 0;

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

        // background
        tft->fillRect(x, y, w, h, colorBrigthness(myBackColor, 0));
        // border
        tft->drawRect(x, y, w, h, colorBrigthness(myBackColor, -55));
        tft->drawRect(x + 1, y + 1, w - 2, h - 2, colorBrigthness(myBackColor, -20));

        // print selectedOption
        tft->setCursor(x + 7, y + ((h - fH) / 2) + 1);

        // Check if text is to long for box
        bool shortened = false;
        char toPrint[100];
        int tXsize = tft->strPixelLen(t);
        toPrint[0] = '\0';
        if (strlen(t) < 100)
            for (uint8_t i = 0; i < strlen(t); i++)
            {
                toPrint[i] = t[i];
                toPrint[i + 1] = '\0';
                tXsize = tft->strPixelLen(toPrint);
                if (tXsize > w - 14 - h)
                {
                    toPrint[i] = '\0';
                    shortened = true;
                    break;
                }
            }

        tft->print(toPrint);
        if (shortened)
            tft->print((char *)"...");

        // down-button
        if (isOpened)
            tft->fillRect(x + w - h, y, h, h, colorBrigthness(myBackColor, -30));
        else
            tft->fillRect(x + w - h, y, h, h, colorBrigthness(myBackColor, -8));
        tft->drawRect(x + w - h, y, h, h, colorBrigthness(myBackColor, -55));
        // down-arrow
        tft->fillTriangle(x + w - h / 2, y + (float)h / 3.0f * 2.0f, x + w - h / 3, y + (float)h / 3.0f, x + w - (float)h / 3.0f * 2.0f, y + (float)h / 3.0f, myForeColor);

        if (isOpened)
        {
            uint8_t margin = 5;

            // dropDown window width
            for (uint8_t i = 0; i < 8; i++)
            {
                if (options[i] != (char *)"\0" && options[i] != NULL)
                {
                    ddWidth = tft->strPixelLen(options[i]);
                    if (ddWidth > maxStrLen)
                        maxStrLen = ddWidth;
                }
            }
            ddWidth = maxStrLen + 2 * margin;

            // dropDown window height
            ddHeight = _optionsCount * (fH + 2 * margin);

            // dropDown window position
            ddX = x;
            ddY = y + h + 2;

            // avoid overflow right
            if (ddX + ddWidth > tft->width() - 1)
            {
                ddX -= ddX + ddWidth - tft->width() - 1;
            }

            // avoid overflow bottom
            if (ddY + ddHeight > tft->height() - 1)
            {
                ddY -= ddY + ddHeight - tft->height() - 1;
            }

            // background
            tft->fillRect(ddX, ddY, ddWidth, ddHeight, colorBrigthness(myBackColor, 0));
            // border
            tft->drawRect(ddX, ddY, ddWidth, ddHeight, colorBrigthness(myBackColor, -55));

            if (_selectedIndex >= 0)
            {
                // highligt selected index
                uint16_t oneHeight = ddHeight / _optionsCount;

                tft->fillRect(ddX + 1, ddY + 1 + _selectedIndex * oneHeight, ddWidth - 2, oneHeight - 1, colorBrigthness(myBackColor, -30));
            }

            for (uint8_t i = 0; i < 8; i++)
            {
                if (options[i] != (char *)"\0" && options[i] != NULL)
                {
                    tft->setCursor(ddX + margin, ddY + margin + (i * (fH + 2 * margin)));
                    tft->print(options[i]);
                    tft->drawLine(ddX, ddY + (i * (fH + 2 * margin)), ddX + ddWidth - 1, ddY + (i * (fH + 2 * margin)), colorBrigthness(myBackColor, -55));
                }
            }
        }
    }

    void internalOnClickHandler(int touchX, int touchY)
    {
        int16_t ptcX = touchX - x;

        if (isOpened)
        {
            // check if dd window is touched
            if (enabled && touchX >= ddX && touchX <= ddX + ddWidth && touchY >= ddY && touchY <= ddY + ddHeight)
            {
                _isTouched = true;
                int16_t ptddY = touchY - ddY; // point of click in dd-window
                uint16_t oneHeight = ddHeight / _optionsCount;
                _selectedIndex = ptddY / oneHeight;
                t = options[_selectedIndex];
                priorized = false;
                isOpened = false;
                return;
            }
        }

        // check if touchX on open button
        if (ptcX >= w - h && ptcX <= w)
        {
            // down touched
            if (isOpened)
            {
                priorized = false;
                isOpened = false;
            }
            else
            {
                priorized = true;
                isOpened = true;
            }
        }
    }

private:
    bool isOpened = false;
    uint16_t maxStrLen = 0;
    int8_t _selectedIndex = -1;
    uint8_t _optionsCount = 0;
    char *options[8];
    uint16_t myForeColor = 0x0000;
    uint16_t myBackColor = 0xFFDF;
};

class TextBox : public Control
{
public:
    TextBox()
    {
        type = TEXTBOX;
    };

    TextBox(int xPos, int yPos, int width, int height, void (*function)() = nullptr)
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
        clickHandler = function;
        t = (char *)"\0";
        type = TEXTBOX;
    };

    char *getText()
    {
        return t;
    }

    void addText(char *text)
    {
        if (text[0] == 8)
        {
            if (strlen(t) > 0)
                t[strlen(t) - 1] = '\0';
        }
        else
        {
            strcpy(str, t);
            strcat(str, text);

            t = str;
        }
    }

    void addText(long text)
    {
        char buf[14];
        ltoa(text, buf, 10);
        strcpy(str, t);
        strcat(str, buf);

        t = str;
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

        // background
        tft->fillRect(x, y, w, h, colorBrigthness(myBackColor, 0));
        // border
        tft->drawRect(x, y, w, h, colorBrigthness(myBackColor, -55));
        tft->drawRect(x + 1, y + 1, w - 2, h - 2, colorBrigthness(myBackColor, -20));

        // value
        tft->setCursor(x + h / 2 - (f.cap_height + 2) / 2, y + ((h - fH) / 2) + 1);

        bool toLong = false;
        bool shortened = false;
        char toPrint[100];
        strcpy(toPrint, t);
        int tXsize = tft->strPixelLen(toPrint);
        if (tXsize > w - 14)
            toLong = true;
        int offset = 0;

        while (toLong)
        {
            for (uint8_t i = 0; i < strlen(t); i++)
            {
                toPrint[i] = t[strlen(t) - i - 1];
                toPrint[i + 1] = '\0';
                tXsize = tft->strPixelLen(toPrint);
                if (tXsize > w - 14)
                {
                    offset = strlen(t) - i + 2;
                    toLong = false;
                    shortened = true;
                    break;
                }
            }
        }

        for (uint8_t i = 0; i < strlen(t) - offset + 1; i++)
        {
            toPrint[i] = t[i + offset];
        }
        toPrint[strlen(t) - offset + 1] = '\0';

        if (shortened)
            tft->print("...");

        tft->print(toPrint);
    }

    void internalOnClickHandler(int touchX, int touchY)
    {
    }

private:
    uint16_t myForeColor = 0x0000;
    uint16_t myBackColor = 0xFFDF;
};

class NumPad : public Control
{
public:
    NumPad()
    {
        x = 0;
        y = 65000;
        w = 0;
        h = 0;

        t = (char *)"\0";
        type = NUMPAD;
        enabled = false;
    };

    NumPad(void (*function)())
    {
        x = 0;
        y = 65000;
        w = 0;
        h = 0;

        clickHandler = function;
        t = (char *)"\0";
        type = NUMPAD;
        enabled = false;
    };

    NumPad(uint16_t yPos, void (*function)())
    {
        x = 0;
        y = yPos;
        w = 0;
        h = 0;

        clickHandler = function;
        t = (char *)"\0";
        type = NUMPAD;
        enabled = false;
    };

    char *getClickedValue()
    {
        return lastClicked;
    }

protected:
    void draw(TFTLIB *tft) override
    {
        if (enabled)
        {
            priorized = true;
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

            if (tft->width() > tft->height()) // landscape --> one line NumPad
            {
                oneW = tft->width() / 13 - 4;
                w = 13 * (oneW + 2);
                h = oneH;
                x = tft->width() / 2 - w / 2;
                if (y == 65000)
                    y = tft->height() - 2 * oneH - 1;

                for (int i = 0; i < 13; i++)
                {
                    // button xPos
                    uint16_t xPos = x + i * (oneW + 2);

                    // shape of button
                    // upper half background
                    tft->fillRect(xPos + 2, y + 2, oneW - 4, h / 2 - 2, colorBrigthness(myBackColor, 0));
                    // lower half background
                    tft->fillRect(xPos + 2, y + h / 2, oneW - 4, h / 2 - 2, colorBrigthness(myBackColor, -25));
                    // transition between the two halves
                    tft->drawLine(xPos + 2, y + h / 2, xPos + oneW - 2, y + h / 2, colorBrigthness(myBackColor, -8));
                    tft->drawLine(xPos + 2, y + 1 + h / 2, xPos + oneW - 2, y + 1 + h / 2, colorBrigthness(myBackColor, -16));
                    // inner border
                    tft->drawRoundRect(xPos + 1, y + 1, oneW - 2, h - 2, 2, colorBrigthness(myBackColor, 11));
                    // outer border
                    tft->drawRoundRect(xPos, y, oneW, h, 3, colorBrigthness(myBackColor, -55));

                    // number
                    tft->setCursor(xPos + (oneW / 2) - 4, y + ((h - fH) / 2) + 1);
                    tft->setTextColor(myForeColor);
                    if (i < 10)
                        tft->print(i);
                    else if (i < 11)
                        tft->print(".");
                    else if (i < 12)
                        tft->print("<");
                    else if (i < 13)
                        tft->print("X");
                }
            }
            else // portrait --> block NumPad
            {
                // TODO
            }
        }
        else
        {
            priorized = false;
        }
    }

    void internalOnClickHandler(int touchX, int touchY)
    {
        float ptcX = touchX - x;
        float ptcY = touchY - y;
        int clickIndex = ptcX / (oneW + 2);

        if (ptcY > 0 && ptcY <= h)
        {
            if (clickIndex >= 12 && clickIndex < 13) // x
            {
                enabled = false;
            }
            else
            {
                if (clickIndex >= 11) //<
                {
                    lastClicked = bs;
                }
                else if (clickIndex >= 10) //.
                {
                    lastClicked = (char *)".";
                }
                else
                {
                    char buf[2];
                    ltoa(clickIndex, buf, 10);
                    lastClicked = buf;
                }

                if (clickHandler != nullptr)
                {
                    clickHandler();
                }
            }
        }
    }

private:
    char bs[2] = {8, 0}; // backspace
    char *lastClicked = '\0';
    uint8_t oneW;
    uint8_t oneH = 30;
    uint16_t myForeColor = 0x0000;
    uint16_t myBackColor = 0xFFDF;
};