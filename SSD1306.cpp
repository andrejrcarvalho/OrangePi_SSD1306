#include "SSD1306.h"

SSD1306::SSD1306(char *i2c_bus, uint8_t i2c_addr)
{
    _i2c_addr = i2c_addr;

    _font = (font_info *)&ubuntuMono_8ptFontInfo;

    _i2c_desc = open(i2c_bus, O_RDWR);

    if (_i2c_desc < 0)
    {
        printf("Failed to open '%s'.\n", i2c_bus);
        /* TODO: check errno to see what went wrong */
        exit(ERRO_INIT_CONNECTION);
    }

    if (ioctl(_i2c_desc, I2C_SLAVE, i2c_addr) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* TODO you can check errno to see what went wrong */
        exit(ERRO_INIT_CONNECTION);
    }

    uint8_t vccstate = SSD1306_SWITCHCAPVCC;

    // Initialisation sequence
    turnOff();
    //  1. set mux ratio
    command(SSD1306_SETMULTIPLEX);
    command(0x3F);
    //  2. set display offset
    command(SSD1306_SETDISPLAYOFFSET);
    command(0x0);
    //  3. set display start line
    command(SSD1306_SETSTARTLINE | 0x0);
    command(SSD1306_MEMORYMODE); // 0x20
    command(0x00);               // 0x0 act like ks0108
    //  4. set Segment re-map A0h/A1h
    command(SSD1306_SEGREMAP | 0x1);
    //   5. Set COM Output Scan Direction C0h/C8h
    command(SSD1306_COMSCANDEC);
    //  6. Set COM Pins hardware configuration DAh, 12
    command(SSD1306_SETCOMPINS);
    command(0x12);
    //  7. Set Contrast Control 81h, 7Fh
    command(SSD1306_SETCONTRAST);
    if (vccstate == SSD1306_EXTERNALVCC)
    {
        command(0x9F);
    }
    else
    {
        command(0xff);
    }
    //  8. Disable Entire Display On A4h
    command(SSD1306_DISPLAYALLON_RESUME);
    //  9. Set Normal Display A6h
    command(SSD1306_NORMALDISPLAY);
    //  10. Set Osc Frequency  D5h, 80h
    command(SSD1306_SETDISPLAYCLOCKDIV);
    command(0x80);
    //  11. Enable charge pump regulator 8Dh, 14h
    command(SSD1306_CHARGEPUMP);
    if (vccstate == SSD1306_EXTERNALVCC)
    {
        command(0x10);
    }
    else
    {
        command(0x14);
    }
    //  12. Display On AFh
    turnOn();
}

void SSD1306::command(uint8_t comm)
{
    uint8_t ptr[2] =   {0x00, comm};
    i2cWrite(ptr, 2);
}

void SSD1306::turnOn()
{
    command(SSD1306_DISPLAYON);
}

void SSD1306::turnOff()
{
    //    CMD(c);
    command(SSD1306_DISPLAYOFF);
}

void SSD1306::clearScreen()
{
    memset(_screen_buffer, 0x00, 1024);
}

void SSD1306::sendData(uint8_t *data, int size)
{
    uint8_t *ptr;

    ptr = (uint8_t *)calloc(size + 1, sizeof(uint8_t)); // memory allocated using calloc, add + com + data
    if (ptr == NULL)
    {
        printf("Error! memory not allocated.\n");
        exit(0);
    }
    *ptr = 0x40; // first send "Control byte"
    memcpy(ptr + 1, data, size);
    command(SSD1306_SETLOWCOLUMN | 0x0);
    command(SSD1306_SETHIGHCOLUMN | 0x0);
    command(SSD1306_SETSTARTLINE | 0x0);
    i2cWrite(ptr, (size + 1));
    free(ptr);
}

void SSD1306::refresh()
{
    command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
    command(SSD1306_SETHIGHCOLUMN | 0x0); // hi col = 0
    command(SSD1306_SETSTARTLINE | 0x0);  // line #0
    sendData(_screen_buffer, sizeof(_screen_buffer));
}

void SSD1306::i2cWrite(uint8_t *buffer, ssize_t size)
{
    if (write(_i2c_desc, buffer, size) != size)
    {
        fprintf(stderr, "Error writing %i bytes\n", size);
        printf(" Value of erro: %s\n ", strerror(errno));
    }
}

void SSD1306::stop()
{
    clearScreen();
    refresh();
    turnOff();
    close(_i2c_desc);
}

void SSD1306::drawPixel(int16_t x, int16_t y, uint8_t color)
{
    if (x >= SSD1306_LCDWIDTH || x < 0 || y >= SSD1306_LCDHEIGHT || y < 0)
        return;

    switch (color)
    {
    case WHITE:
        _screen_buffer[x + (y / 8) * SSD1306_LCDWIDTH] |= (1 << (y & 7));
        break;
    case BLACK:
        _screen_buffer[x + (y / 8) * SSD1306_LCDWIDTH] &= ~(1 << (y & 7));
        break;
    case INVERSE:
        _screen_buffer[x + (y / 8) * SSD1306_LCDWIDTH] ^= (1 << (y & 7));
        break;
    }
}

void SSD1306::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t steep = (abs(y1 - y0) > abs(x1 - x0));
    if (steep)
    {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }

    if (x0 > x1)
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (steep)
        {
            drawPixel(y0, x0, color);
        }
        else
        {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

void SSD1306::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    int16_t x1, y1;
    if ((w == 0) | (h == 0))
        return;
    x1 = x + w - 1;
    y1 = y + h - 1;

    if ((h > 2) | (w > 2))
    {
        drawLine(x, y, x1, y, color);
        drawLine(x, y1, x1, y1, color);
        drawLine(x, y + 1, x, y1 - 1, color);
        drawLine(x1, y + 1, x1, y1 - 1, color);
    }
    else
    {
        drawLine(x, y, x1, y, color);
        drawLine(x, y1, x1, y1, color);
    }
}

uint8_t SSD1306::drawChar(int16_t x, int16_t y, unsigned char c, uint8_t font_size, uint16_t color)
{
    int16_t i, j, k, _x;
    uint16_t line;

    if ((c < _font->startChar) || // skip if character don't exist
        (c > _font->endChar))     // skip if character don't exist
        return 0;

    c = c - _font->startChar;
    // skip invisible characters
    line = _font->charInfo[c].offset;
    // scan height
    for (i = 0; i < _font->charInfo[c].heightBits; i++)
    {
        k = (_font->charInfo[c].widthBits - 1) / 8 + 1; //number of bytes in a row
        _x = 0;
        do
        {
            int16_t l = _font->data[line];
            // scan width
            for (j = 0; j < 8; j++)
            {
                if (l & 0x80)
                {
                    if (font_size == 1)
                    {
                        drawPixel(x + _x, y + i, color);
                    }
                    else
                    {
                        drawRect(x + (_x * font_size), y + (i * font_size), font_size, font_size, color);
                    }
                }
                l <<= 1;
                _x++;
            }
            k--;
            line++;
        } while (k > 0);
    }
    return _font->charInfo[c].widthBits;
}

void SSD1306::drawString(int16_t x, int16_t y, char *text, uint8_t font_size, uint16_t color)
{
    uint16_t l, pos;
    pos = 0;
    for (l = 0; l < strlen(text); l++)
    {
        pos = pos + drawChar(x + (pos * font_size + 1), y, text[l], font_size, color);
    }
}

void SSD1306::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}
