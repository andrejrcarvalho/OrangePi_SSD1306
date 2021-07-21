#ifndef SSD1306_H
#define SSD1306_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include "font.h"
#include "ubuntuMono8pt.h"

//Errors
#define ERRO_INIT_CONNECTION 10

#define     BLACK                       0
#define     WHITE                       1
#define     INVERSE                     2


#define     SSD1306_LCDWIDTH            128
#define     SSD1306_LCDHEIGHT          	64

// Commands
#define SSD1306_SETCONTRAST             0x81
#define SSD1306_DISPLAYALLON_RESUME     0xA4
#define SSD1306_DISPLAYALLON            0xA5
#define SSD1306_NORMALDISPLAY           0xA6
#define SSD1306_INVERTDISPLAY           0xA7
#define SSD1306_DISPLAYOFF              0xAE
#define SSD1306_DISPLAYON               0xAF
#define SSD1306_SETDISPLAYOFFSET        0xD3
#define SSD1306_SETCOMPINS              0xDA
#define SSD1306_SETVCOMDETECT           0xDB
#define SSD1306_SETDISPLAYCLOCKDIV      0xD5
#define SSD1306_SETPRECHARGE            0xD9
#define SSD1306_SETMULTIPLEX            0xA8
#define SSD1306_SETLOWCOLUMN            0x00
#define SSD1306_SETHIGHCOLUMN           0x10
#define SSD1306_SETSTARTLINE            0x40
#define SSD1306_MEMORYMODE              0x20
#define SSD1306_COMSCANINC              0xC0
#define SSD1306_COMSCANDEC              0xC8
#define SSD1306_SEGREMAP                0xA0
#define SSD1306_CHARGEPUMP              0x8D
#define SSD1306_EXTERNALVCC             0x1
#define SSD1306_SWITCHCAPVCC            0x2

class SSD1306 {
  public:
    void command(uint8_t comm);
    void turnOn();
    void turnOff();
    void clearScreen();
    void drawPixel(int16_t x, int16_t y, uint8_t color);
    void refresh();
    uint8_t drawChar(int16_t x, int16_t y, unsigned char c, uint8_t font_size, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawString(int16_t x, int16_t y, char *text, uint8_t font_size, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void stop();
    SSD1306(char * i2c_bus, uint8_t i2c_addr);
  private:
    uint8_t _i2c_addr;
    int _i2c_desc;
    uint8_t _screen_buffer[SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8];
    font_info *_font;
    void sendData(uint8_t *data, int size);
    void i2cWrite( uint8_t * buffer, ssize_t size);
    void swap(int16_t * a , int16_t * b) { int16_t t = *a; *a = *b; *b = t; }
};


#endif