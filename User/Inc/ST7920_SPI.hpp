#ifndef _ST7920_SPI_H
#define _ST7920_SPI_H 

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"
#include "main.h"
#include "stdio.h"
#include "sub_function.hpp"
// ST7920 Commands

#define LCD_CLS         0x01
#define LCD_HOME        0x02
#define LCD_ADDRINC     0x06
#define LCD_DISPLAYON   0x0C
#define LCD_DISPLAYOFF  0x08
#define LCD_CURSORON    0x0E
#define LCD_CURSORBLINK 0x0F
#define LCD_BASIC       0x30
#define LCD_EXTEND      0x34
#define LCD_GFXMODE     0x36
#define LCD_TXTMODE     0x34
#define LCD_STANDBY     0x01
#define LCD_SCROLL      0x03
#define LCD_SCROLLADDR  0x40
#define LCD_ADDR        0x80//0x80
#define LCD_LINE0       0x80
#define LCD_LINE1       0x90
#define LCD_LINE2       0x88
#define LCD_LINE3       0x98

#define SPI_SPEED (1000000UL)

#ifndef SCR_WD
#define SCR_WD  192
#define SCR_HT  64
#define SCR_LEFT_WD	128
#define SCR_LEFT_HT	32
#define SCR_RIGHT_WD	64
#define SCR_RIGHT_HT	32
#endif

#define ALIGN_LEFT    0
#define ALIGN_RIGHT  -1 
#define ALIGN_CENTER -2

typedef struct _propFont
{
  const uint8_t* font;
  int8_t xSize;
  uint8_t ySize;
  uint8_t firstCh;
  uint8_t lastCh;
  uint8_t minCharWd;
  uint8_t minDigitWd;
}_propFont;

// ---------------------------------
class ST7920_SPI
{
public:
	ST7920_SPI();

  void init();
  void sendCmd(uint8_t b, uint8_t num);
  void sendData(uint8_t b, uint8_t num);
  void switchBuf(uint8_t b);
  void display(int buf=0);
  void gotoXY(uint8_t x, uint8_t y, uint8_t num);
  void sleep(bool mode=true);
  void setGfxMode(bool mode=true);
  void cls();
  void drawPixel(uint8_t x, uint8_t y, uint8_t col);
  void drawLineVfast(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col);
  void drawLineVfastD(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col);
  void drawLineHfast(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col);
  void drawLineHfastD(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col);
  void drawRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col);
  void drawRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col);
  void fillRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col);
  void fillRectD(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col);
  void drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t col);
  void fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col);
  void fillCircleD(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col);
  void setDither(uint8_t s);
  int drawBitmap(const uint8_t *bmp, int x, uint8_t y, uint8_t w, uint8_t h);
  int drawBitmap(const uint8_t *bmp, int x, uint8_t y);
  int printStr(int xpos, int ypos, char *str, bool del = false);
  int printStrInvert(int xpos, int ypos, char *str, bool del = false);
  void setFont(const uint8_t* f);
  void invertString(uint8_t x, uint8_t y, uint8_t length, uint8_t offsetLength=0);
  void deleteString(uint8_t x, uint8_t y, uint8_t length, uint8_t offsetLength=0);
  void drawProgressBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t percent);
  _propFont  cfont;
  int strWidth(char *txt);
  void setBackLight(uint8_t _level);
private:
  void splitScreen(void);

  void setCR(uint8_t _cr) { cr = _cr; }
  void setInvert(uint8_t _inv) { invertCh = _inv; }
  void setFontMinWd(uint8_t wd) { cfont.minCharWd = wd; }
  void setCharMinWd(uint8_t wd) { cfont.minCharWd = wd; }
  void setDigitMinWd(uint8_t wd) { cfont.minDigitWd = wd; }
  int printChar(int xpos, int ypos, unsigned char c);
  int printCharInvert(int xpos, int ypos, unsigned char c, unsigned char cnext);
  int charWidth(uint8_t _ch, bool last=true);

  unsigned char convertPolish(unsigned char _c);
  static bool isdigit(uint8_t ch);
  uint16_t max(uint16_t x,uint16_t y);
  static bool isNumber(uint8_t ch);
  static bool isNumberExt(uint8_t ch);
  void setIsNumberFun(bool (*fun)(uint8_t)) { isNumberFun=fun; }
  void SPI_SendByte(uint8_t data, uint8_t num);
  void clearSreen(void);


  uint8_t scr[SCR_WD*SCR_HT/8] = {0};
  uint8_t upLeftScr[SCR_LEFT_WD*SCR_LEFT_HT/8] = {0};
  uint8_t upRightScr[SCR_RIGHT_WD*SCR_RIGHT_HT/8] = {0};
  uint8_t downLeftScr[SCR_LEFT_WD*SCR_LEFT_HT/8] = {0};
  uint8_t downRightScr[SCR_RIGHT_WD*SCR_RIGHT_HT/8] = {0};
  uint8_t scrWd = 128/8;
  uint8_t scrHt = 64;
  uint8_t csPin  = 10;

  static uint8_t xstab[8];
  static uint8_t xetab[8];
  static uint8_t pattern[4];
  static const uint8_t ditherTab[4*17];

//private:
  bool (*isNumberFun)(uint8_t ch);

  uint8_t cr;  // carriage return mode for printStr
  uint8_t dualChar;
  uint8_t invertCh;
  uint8_t spacing = 1;
};
#endif

