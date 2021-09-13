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

public:
    Control(){};

    virtual void draw(TFTLIB *tft){

    };

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

    void setTextColor(uint16_t color)
    {
        fgColor = color;
    };

    void setBgColor(uint16_t color)
    {
        bgColor = color;
    };

    uint8_t getType()
    {
        return type;
    }

    bool checkTouched(int touchX, int touchY, int touchZ)
    {
        if (enabled && touchX >= x && touchX <= x + w && touchY >= y && touchY <= y + h && touchZ > 0)
        {
            if (clickHandler != nullptr)
            {
                clickHandler();
            }

            internalOnClickHandler();
        }
        else
        {
            _isTouched = false;
        }

        return _isTouched;
    }

protected:
    bool enabled = true;
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t w = 50;
    uint16_t h = 20;
    FONTS f = Arial_12;
    uint8_t fH = 14;
    char *t = (char *)"Undefined";
    uint16_t fgColor = 0x0000;
    uint16_t bgColor = 0xF79E;
    bool _isTouched = false;
    void (*clickHandler)() = nullptr;
    uint8_t type = UNDEFINED;

    virtual void internalOnClickHandler()
    {
        _isTouched = true;
    }

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

    void draw(TFTLIB *tft) override
    {
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
        tft->fillRect(x + 2, y + 2, w - 4, h / 2 - 2, colorBrigthness(bgColor, darken));
        //lower half background
        tft->fillRect(x + 2, y + h / 2, w - 4, h / 2 - 2, colorBrigthness(bgColor, -25 + darken));
        //transition between the two halves
        tft->drawLine(x + 2, y + h / 2, x + w - 2, y + h / 2, colorBrigthness(bgColor, -8 + darken));
        tft->drawLine(x + 2, y + 1 + h / 2, x + w - 2, y + 1 + h / 2, colorBrigthness(bgColor, -16 + darken));
        //inner border
        tft->drawRoundRect(x + 1, y + 1, w - 2, h - 2, 2, colorBrigthness(bgColor, +11 + darken));
        //outer border
        tft->drawRoundRect(x, y, w, h, 3, colorBrigthness(bgColor, -55 + darken));

        //Text
        if (sizeof(t) > 1)
        {
            tft->setCursor(x + (w / 2) - (tXsize / 2), y + ((h - fH) / 2));
            if (enabled)
                tft->setTextColor(fgColor);
            else
                tft->setTextColor(0xA534);
            tft->print(t);
        }
    }
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

    void draw(TFTLIB *tft) override
    {
        tft->setFont(f);
        tft->setTextColor(fgColor);
        tft->setCursor(x, y);
        tft->print(t);
    }
};

class CheckBox : public Control
{
public:
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

    void internalOnClickHandler()
    {
        checked = !checked;
    }

    bool checked = false;

    void draw(TFTLIB *tft) override
    {
        tft->setFont(f);

        //the box
        tft->fillRect(x, y, 16, 16, bgColor);
        tft->drawRect(x, y, 16, 16, fgColor);

        //the hook
        if (checked)
        {
            tft->drawLine(x + 2, y + 7, x + 6, y + 11, fgColor);
            tft->drawLine(x + 3, y + 7, x + 6, y + 10, fgColor);
            tft->drawLine(x + 7, y + 9, x + 12, y + 4, fgColor);
            tft->drawLine(x + 7, y + 10, x + 13, y + 4, fgColor);
        }

        //the label
        tft->setTextColor(fgColor);
        tft->setCursor(x + 22, y + ((h - fH) / 2) + 1);
        tft->print(t);
    }
};