#include <ST7920_SPI.hpp>

#define CONVERT_POLISH
#define fontbyte(x) *(cfont.font+(x))
#define TIMER_US				&htim13
extern TIM_HandleTypeDef htim13;

// ----------------------------------------------------------------
ST7920_SPI::ST7920_SPI()
{
  //csPin  = cs;
}


void delayUs(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(TIMER_US, 0);
	while(__HAL_TIM_GET_COUNTER(TIMER_US) < us);
}


void ST7920_SPI::SPI_SendByte(uint8_t data, uint8_t num)
{
	for(int i=0;i<8;i++)
	{
		if((data<<i)&0x80)
				HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port, LCD_MOSI_Pin, GPIO_PIN_SET);  // SID=1  OR MOSI
		else
			HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port, LCD_MOSI_Pin, GPIO_PIN_RESET);  // SID=0

		switch(num)
		{
		case 0:
			HAL_GPIO_WritePin(LCD_SCLK1_GPIO_Port, LCD_SCLK1_Pin, GPIO_PIN_RESET);  // SCLK =0  OR SCK
			HAL_GPIO_WritePin(LCD_SCLK1_GPIO_Port, LCD_SCLK1_Pin, GPIO_PIN_SET);  // SCLK=1
			break;
		case 1:
			HAL_GPIO_WritePin(LCD_SCLK2_GPIO_Port, LCD_SCLK2_Pin, GPIO_PIN_RESET);  // SCLK =0  OR SCK
			HAL_GPIO_WritePin(LCD_SCLK2_GPIO_Port, LCD_SCLK2_Pin, GPIO_PIN_SET);  // SCLK=1
			break;
		}
	}

}

void ST7920_SPI::setBackLight(uint8_t _level)
{
	int16_t _bright = _level*20 - 1;
	if(_bright > 99)
		_bright = 99;
	else if(_bright < 0)
		_bright = 0;
	__HAL_TIM_SET_COMPARE(&LCD_PWM, LCD_PWM_CHANNEL, _bright);
}

uint16_t ST7920_SPI::max(uint16_t x,uint16_t y)
{
	if(x>y)
		return x;
	else
		return y;
}
// ----------------------------------------------------------------
void ST7920_SPI::init()
{
	HAL_Delay(100);
  scrWd=SCR_WD/8;
  scrHt=SCR_HT;
  isNumberFun = &isNumber;
  cr = 0;
  cfont.font = NULL;
  dualChar = 0;
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
  sendCmd(LCD_BASIC, 0);
  sendCmd(LCD_BASIC, 1);
  HAL_Delay(1);
  sendCmd(LCD_BASIC, 0);
  sendCmd(LCD_BASIC, 1);
  HAL_Delay(1);
  sendCmd(LCD_CLS, 0);
  sendCmd(LCD_CLS, 1);
  HAL_Delay(12);
  sendCmd(LCD_ADDRINC, 0);
  sendCmd(LCD_ADDRINC, 1);
  HAL_Delay(1);
  sendCmd(LCD_DISPLAYON, 0);
  sendCmd(LCD_DISPLAYON, 1);
  HAL_Delay(1);
  setGfxMode(true);
  HAL_Delay(1);
}
// ----------------------------------------------------------------
void ST7920_SPI::sendCmd(uint8_t b, uint8_t num)
{
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
  SPI_SendByte(0xF8, num);
  SPI_SendByte(b & 0xF0, num);
  SPI_SendByte(b << 4, num);
  delayUs(50);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
}
// ----------------------------------------------------------------

void ST7920_SPI::sendData(uint8_t b, uint8_t num)
{
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
  SPI_SendByte(0xFA, num);
  SPI_SendByte(b & 0xF0, num);
  SPI_SendByte(b << 4, num);
  delayUs(50);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
}


// ----------------------------------------------------------------
// y =  0..63  -> buffer #0
// y = 64..127 -> buffer #1
void ST7920_SPI::gotoXY(uint8_t x, uint8_t y, uint8_t num)
{
  if(y>=32 && y<64) { y-=32; x+=8; } else
  if(y>=64 && y<64+32) { y-=32; x+=0; } else
  if(y>=64+32 && y<64+64) { y-=64; x+=8; } else
  if(y>=64+64&&y<64+128) { y-=64; x+=0;}
  sendCmd(LCD_ADDR | y, num); // 6-bit (0..63)
  sendCmd(LCD_ADDR | x, num); // 4-bit (0..15)
}
// ----------------------------------------------------------------
// true - graphics mode
// false - text mode
void ST7920_SPI::setGfxMode(bool mode)
{
  if(mode) {
    sendCmd(LCD_EXTEND,0);
    sendCmd(LCD_EXTEND,1);
    HAL_Delay(1);
    sendCmd(LCD_GFXMODE,0);
    sendCmd(LCD_GFXMODE,1);

  }
  else
  {
    sendCmd(LCD_EXTEND,0);
    sendCmd(LCD_EXTEND,1);
    HAL_Delay(1);
    sendCmd(LCD_TXTMODE,0);
    sendCmd(LCD_TXTMODE,1);
  }
}
// ----------------------------------------------------------------
void ST7920_SPI::sleep(bool mode)
{
  //sendCmd(mode?LCD_STANDBY:LCD_EXTEND);
  if(mode)
  {
  	sendCmd(LCD_STANDBY, 0);
  	sendCmd(LCD_STANDBY, 1);
  }

  else
  {
  	sendCmd(LCD_BASIC, 0);
  	sendCmd(LCD_BASIC, 1);
  	sendCmd(LCD_EXTEND, 0);
  	sendCmd(LCD_EXTEND, 1);
  	sendCmd(LCD_GFXMODE, 0);
  	sendCmd(LCD_GFXMODE, 1);
  }
}
// ----------------------------------------------------------------
void ST7920_SPI::switchBuf(uint8_t b)
{
  sendCmd(LCD_SCROLL, 0);
  sendCmd(LCD_SCROLL, 1);
  sendCmd(LCD_SCROLLADDR+32*b, 0);
  sendCmd(LCD_SCROLLADDR+32*b, 1);
}
// ----------------------------------------------------------------
// Arduino Uno: 33-34ms -> 29fps
// optimized for full 128x64 frame
// 0 - buffer #0
// 1 - buffer #1

void ST7920_SPI::splitScreen(void)
{
	uint16_t _index1, _index2, _index3, _index4, _halfScreen;
	for(int j = 0;  j < SCR_LEFT_HT; j++)
	{
		//array up lef 128x32
		_index1 = j*SCR_LEFT_WD/8;
		_index2 = j*SCR_RIGHT_WD/8;
		for(int i = 0; i < SCR_LEFT_WD/8; i++)
		{
		  upLeftScr[_index1+i] = scr[_index1+_index2+i];
		}
		//
		_halfScreen = SCR_WD*SCR_HT/8/2;
		for(int u = 0; u < SCR_LEFT_WD/8; u++)
		{
		  downLeftScr[_index1+u] = scr[_index1+_index2+u+_halfScreen];
		}
		// array up right 64x32
		_index3 = j*SCR_RIGHT_WD/8;
		_index4 = (j+1)*SCR_LEFT_WD/8;
		for(int k = 0; k < SCR_RIGHT_WD/8; k++)
		{
			upRightScr[_index3+k] = scr[_index3+_index4+k];
		}
		for(int k = 0; k < SCR_RIGHT_WD/8; k++)
		{
			downRightScr[_index3+k] = scr[_index3+_index4+k+_halfScreen];
		}
	}

}


//1 frame ~ 30ms (33.33 fps)
void ST7920_SPI::display(int buf)
{
  splitScreen();
  uint8_t _byte = 0, _num = 0;
  for(int y=0;y<32;y++)
  {
  		// up screen
  		_num = 0;
      sendCmd(LCD_LINE0 | y, _num);
      sendCmd(LCD_LINE0 | 0, _num);
      HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
      SPI_SendByte(0xFA, _num);
      //send 16 bytes left screen
      for(int x=0;x<16;x++)
      {
      	_byte = upLeftScr[x+y*16];

        SPI_SendByte(_byte & 0xF0, _num);
        SPI_SendByte(_byte << 4, _num);
      }
      //send 8 bytes right screen
      for(int x=0; x<8; x++)
      {
      	_byte = upRightScr[x+y*8];
        SPI_SendByte(_byte & 0xF0, _num);
        SPI_SendByte(_byte << 4, _num);
      }
      HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
      // down screen
  		_num = 1;
      sendCmd(LCD_LINE0 | (y), _num);
      sendCmd(LCD_LINE0 | 0, _num);
      HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
      SPI_SendByte(0xFA, _num);
      for(int x=0;x<16;x++)
      {
      	_byte = downLeftScr[x+(y)*16];
        SPI_SendByte(_byte & 0xF0, _num);
        SPI_SendByte(_byte << 4, _num);
      }
      for(int x=0; x<8; x++)
      {
      	_byte = downRightScr[x+(y)*8];
        SPI_SendByte(_byte & 0xF0, _num);
        SPI_SendByte(_byte << 4, _num);
      }
      HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

  }
}

void ST7920_SPI::cls()
{
  memset(scr,0,SCR_WD*SCR_HT/8);
}

// ----------------------------------------------------------------
void ST7920_SPI::clearSreen(void)
{
	sendCmd(LCD_CLS, 0);
	sendCmd(LCD_CLS, 1);
	HAL_Delay(12);
	cls();
}

// ----------------------------------------------------------------
void ST7920_SPI::drawPixel(uint8_t x, uint8_t y, uint8_t col)
{
  if(x>=SCR_WD || y>=SCR_HT) return;
  switch(col) {
    case 1: scr[y*scrWd+x/8] |=   (0x80 >> (x&7)); break;
    case 0: scr[y*scrWd+x/8] &=  ~(0x80 >> (x&7)); break;
    case 2: scr[y*scrWd+x/8] ^=   (0x80 >> (x&7)); break;
  }
}


// ----------------------------------------------------------------
// about 4x faster than regular drawLineV
void ST7920_SPI::drawLineVfast(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col)
{
  uint8_t mask = 0x80 >> (x&7);
  if(y1<y0) {
    mask=y0; y0=y1; y1=mask; // swap
  }
  mask = 0x80 >> (x&7);
  switch(col) {
    case 1: for(int y=y0; y<=y1; y++) scr[y*scrWd+x/8] |= mask;   break;
    case 0: for(int y=y0; y<=y1; y++) scr[y*scrWd+x/8] &= ~mask;  break;
    case 2: for(int y=y0; y<=y1; y++) scr[y*scrWd+x/8] ^= mask;   break;
  }
}
// ----------------------------------------------------------------
// limited to pattern #8
void ST7920_SPI::drawLineVfastD(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col)
{
  uint8_t mask = 0x80 >> (x&7);
  if(y1<y0) {
    mask=y0; y0=y1; y1=mask; // swap
  }
  if(((x&1)==1 && (y0&1)==0) ||( (x&1)==0 && (y0&1)==1)) y0++;
  mask = 0x80 >> (x&7);
  switch(col) {
    case 1: for(int y=y0; y<=y1; y+=2) scr[y*scrWd+x/8] |= mask;   break;
    case 0: for(int y=y0; y<=y1; y+=2) scr[y*scrWd+x/8] &= ~mask;  break;
    case 2: for(int y=y0; y<=y1; y+=2) scr[y*scrWd+x/8] ^= mask;   break;
  }
}
// ----------------------------------------------------------------
uint8_t ST7920_SPI::xstab[8]={0xff,0x7f,0x3f,0x1f,0x0f,0x07,0x03,0x01};
uint8_t ST7920_SPI::xetab[8]={0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff};
uint8_t ST7920_SPI::pattern[4]={0xaa,0x55,0xaa,0x55};
// about 40x faster than regular drawLineH
void ST7920_SPI::drawLineHfast(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col)
{
  int yadd=y*scrWd;
  int x8s,x8e;
  if(x1>=x0) {
    x8s=x0/8;
    x8e=x1/8;
  } else {
    x8s=x1; x1=x0; x0=x8s; // swap
    x8s=x1/8;
    x8e=x0/8;
  }
  switch(col) {
    case 1: 
      if(x8s==x8e)
      	scr[yadd+x8s]|=(xstab[x0&7] & xetab[x1&7]);
      else
      {
      	scr[yadd+x8s]|=xstab[x0&7];
      	scr[yadd+x8e]|=xetab[x1&7];
      }
      for(int x=x8s+1; x<x8e; x++)
      	scr[yadd+x]=0xff;
      break;
    case 0:
      if(x8s==x8e) scr[yadd+x8s]&=~(xstab[x0&7] & xetab[x1&7]);
      else { scr[yadd+x8s]&=~xstab[x0&7]; scr[yadd+x8e]&=~xetab[x1&7]; }
      for(int x=x8s+1; x<x8e; x++) scr[yadd+x]=0x00; 
      break;
    case 2: 
      if(x8s==x8e) scr[yadd+x8s]^=(xstab[x0&7] & xetab[x1&7]);
      else { scr[yadd+x8s]^=xstab[x0&7]; scr[yadd+x8e]^=xetab[x1&7]; }
      for(int x=x8s+1; x<x8e; x++) scr[yadd+x]^=0xff;
      break;
  }
}
// ----------------------------------------------------------------
// dithered version
void ST7920_SPI::drawLineHfastD(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col)
{
  int yadd=y*scrWd;
  int x8s,x8e;
  if(x1>=x0) {
    x8s=x0/8;
    x8e=x1/8;
  } else {
    x8s=x1; x1=x0; x0=x8s; // swap
    x8s=x1/8;
    x8e=x0/8;
  }
  switch(col) {
    case 1: 
      if(x8s==x8e) scr[yadd+x8s]|=(xstab[x0&7] & xetab[x1&7] & pattern[y&3]);
      else { scr[yadd+x8s]|=(xstab[x0&7] & pattern[y&3]); scr[yadd+x8e]|=(xetab[x1&7] & pattern[y&3]); }
      for(int x=x8s+1; x<x8e; x++) scr[yadd+x]=pattern[y&3];
      break;
    case 0:
      if(x8s==x8e) scr[yadd+x8s]&=~(xstab[x0&7] & xetab[x1&7] & pattern[y&3]);
      else { scr[yadd+x8s]&=~(xstab[x0&7] & pattern[y&3]); scr[yadd+x8e]&=~(xetab[x1&7] & pattern[y&3]); }
      for(int x=x8s+1; x<x8e; x++) scr[yadd+x]=~pattern[y&3];
      break;
    case 2: 
      if(x8s==x8e) scr[yadd+x8s]^=(xstab[x0&7] & xetab[x1&7] & pattern[y&3]);
      else { scr[yadd+x8s]^=(xstab[x0&7] & pattern[y&3]); scr[yadd+x8e]^=(xetab[x1&7] & pattern[y&3]); }
      for(int x=x8s+1; x<x8e; x++) scr[yadd+x]^=pattern[y&3];
      break;
  }
}
// ----------------------------------------------------------------
void ST7920_SPI::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
  if(x>=SCR_WD || y>=SCR_HT) return;
  uint8_t drawVright=1;
  if(x+w>SCR_WD) { w=SCR_WD-x; drawVright=0; }
  if(y+h>SCR_HT) h=SCR_HT-y; else drawLineHfast(x, x+w-1, y+h-1,col);
  drawLineHfast(x, x+w-1, y,col);
  drawLineVfast(x,    y+1, y+h-2,col);
  if(drawVright) drawLineVfast(x+w-1,y+1, y+h-2,col);
}
// ----------------------------------------------------------------
// dithered version (50% of brightness)
void ST7920_SPI::drawRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
  if(x>=SCR_WD || y>=SCR_HT) return;
  uint8_t drawVright=1;
  if(x+w>SCR_WD) { w=SCR_WD-x; drawVright=0; }
  if(y+h>SCR_HT) h=SCR_HT-y; else drawLineHfastD(x, x+w-1, y+h-1,col);
  drawLineHfastD(x, x+w-1, y,col);
  drawLineVfastD(x,    y+1, y+h-2,col);
  if(drawVright) drawLineVfastD(x+w-1,y+1, y+h-2,col);
}
// ----------------------------------------------------------------
void ST7920_SPI::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
	if(w == 0)
		return;
  if(x>=SCR_WD || y>=SCR_HT) return;
  if(x+w>SCR_WD) w=SCR_WD-x;
  if(y+h>SCR_HT) h=SCR_HT-y;
  for(int i=y;i<y+h;i++)
  {
  	drawLineHfast(x,x+w-1,i,col);
  }

}
// ----------------------------------------------------------------
// dithered version (50% of brightness)
void ST7920_SPI::fillRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
  if(x>=SCR_WD || y>=SCR_HT) return;
  if(x+w>=SCR_WD) w=SCR_WD-x;
  if(y+h>=SCR_HT) h=SCR_HT-y;
  for(int i=y;i<y+h;i++) drawLineHfastD(x,x+w-1,i,col);
}
// ----------------------------------------------------------------
// circle
void ST7920_SPI::drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t col)
{
  int f = 1 - (int)radius;
  int ddF_x = 1;
  int ddF_y = -2 * (int)radius;
  int x = 0;
  int y = radius;
 
  drawPixel(x0, y0 + radius, col);
  drawPixel(x0, y0 - radius, col);
  drawPixel(x0 + radius, y0, col);
  drawPixel(x0 - radius, y0, col);
 
  while(x < y) {
    if(f >= 0) {
      y--; ddF_y += 2; f += ddF_y;
    }
    x++; ddF_x += 2; f += ddF_x;    
    drawPixel(x0 + x, y0 + y, col);
    drawPixel(x0 - x, y0 + y, col);
    drawPixel(x0 + x, y0 - y, col);
    drawPixel(x0 - x, y0 - y, col);
    drawPixel(x0 + y, y0 + x, col);
    drawPixel(x0 - y, y0 + x, col);
    drawPixel(x0 + y, y0 - x, col);
    drawPixel(x0 - y, y0 - x, col);
  }
}
// ----------------------------------------------------------------
void ST7920_SPI::fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col)
{
  drawLineHfast(x0-r, x0-r+2*r+1, y0, col);
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    drawLineHfast(x0-x, x0-x+2*x+1, y0+y, col);
    drawLineHfast(x0-y, x0-y+2*y+1, y0+x, col);
    drawLineHfast(x0-x, x0-x+2*x+1, y0-y, col);
    drawLineHfast(x0-y, x0-y+2*y+1, y0-x, col);
  }
}
// ----------------------------------------------------------------
// dithered version (50% of brightness)
void ST7920_SPI::fillCircleD(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col)
{
  drawLineHfastD(x0-r, x0-r+2*r+1, y0, col);
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    drawLineHfastD(x0-x, x0-x+2*x+1, y0+y, col);
    drawLineHfastD(x0-y, x0-y+2*y+1, y0+x, col);
    drawLineHfastD(x0-x, x0-x+2*x+1, y0-y, col);
    drawLineHfastD(x0-y, x0-y+2*y+1, y0-x, col);
  }
}
// ----------------------------------------------------------------

const uint8_t ST7920_SPI::ditherTab[4*17] = {
  0x00,0x00,0x00,0x00, // 0

  0x88,0x00,0x00,0x00, // 1
  0x88,0x00,0x22,0x00, // 2
  0x88,0x00,0xaa,0x00, // 3
  0xaa,0x00,0xaa,0x00, // 4
  0xaa,0x44,0xaa,0x00, // 5
  0xaa,0x44,0xaa,0x11, // 6
  0xaa,0x44,0xaa,0x55, // 7

  0xaa,0x55,0xaa,0x55, // 8

  0xaa,0xdd,0xaa,0x55, // 9
  0xaa,0xdd,0xaa,0x77, // 10
  0xaa,0xdd,0xaa,0xff, // 11
  0xaa,0xff,0xaa,0xff, // 12
  0xbb,0xff,0xaa,0xff, // 13
  0xbb,0xff,0xee,0xff, // 14
  0xbb,0xff,0xff,0xff, // 15

  0xff,0xff,0xff,0xff  // 16
};

void ST7920_SPI::setDither(uint8_t s)
{
  if(s>16) return;
  pattern[0] = *(ditherTab+s*4+0);
  pattern[1] = *(ditherTab+s*4+1);
  pattern[2] = *(ditherTab+s*4+2);
  pattern[3] = *(ditherTab+s*4+3);
}
// ----------------------------------------------------------------
#define ALIGNMENT \
  if(x==-1) x = SCR_WD-w; \
  else if(x<0) x = (SCR_WD-w)/2; \
  if(x<0) x=0; \
  if(x>=SCR_WD || y>=SCR_HT) return 0; \
  if(x+w>SCR_WD) w = SCR_WD-x; \
  if(y+h>SCR_HT) h = SCR_HT-y
// ----------------------------------------------------------------

int ST7920_SPI::drawBitmap(const uint8_t *bmp, int x, uint8_t y, uint8_t w, uint8_t h)
{
  uint8_t wdb = w;
  ALIGNMENT;
  uint8_t i,j8,d,b,ht8=(h+7)/8;
  for(j8=0; j8<ht8; j8++) {
    for(i=0; i<w; i++) {
      uint8_t mask = 0x80 >> ((x+i)&7);
      d = *(bmp+wdb*j8+i);
      int lastbit = h - j8 * 8;
      if (lastbit > 8) lastbit = 8;
      for(b=0; b<lastbit; b++) {
         if(d & 1) scr[(y+j8*8+b)*scrWd+(x+i)/8] |= mask;
         d>>=1;
      }
    }
  }
  return x+w;
}
// ----------------------------------------------------------------
int ST7920_SPI::drawBitmap(const uint8_t *bmp, int x, uint8_t y)
{
  uint8_t w = *(bmp+0);
  uint8_t h = *(bmp+1);
  return drawBitmap(bmp+2, x, y, w, h);
}
// ----------------------------------------------------------------
// text rendering
// ----------------------------------------------------------------
void ST7920_SPI::setFont(const uint8_t* font)
{
  cfont.font = font;
  cfont.xSize = -fontbyte(0);
  cfont.ySize = fontbyte(1);
  cfont.firstCh = fontbyte(2);
  cfont.lastCh  = fontbyte(3);
  cfont.minDigitWd = 0;
  cfont.minCharWd = 0;
  isNumberFun = &isNumber;
  spacing = 1;
  cr = 0;
  invertCh = 0;
}
// ----------------------------------------------------------------
int ST7920_SPI::charWidth(uint8_t c, bool last)
{
  c = convertPolish(c);
  if (c < cfont.firstCh || c > cfont.lastCh)
      return c==' ' ?  1 + cfont.xSize/2 : 0;
  if (cfont.xSize > 0) return cfont.xSize;
  int ys8=(cfont.ySize+7)/8;
  int idx = 4 + (c-cfont.firstCh)*(-cfont.xSize*ys8+1);
  int wd = *(cfont.font + idx);
  int wdL = 0, wdR = spacing; // default spacing before and behind char
  if((*isNumberFun)(c)) {
    if(cfont.minDigitWd>wd) {
      wdL = (cfont.minDigitWd-wd)/2;
      wdR += (cfont.minDigitWd-wd-wdL);
    }
  } else
  if(cfont.minCharWd>wd) {
    wdL = (cfont.minCharWd-wd)/2;
    wdR += (cfont.minCharWd-wd-wdL);
  }
  return last ? wd+wdL+wdR : wd+wdL+wdR-spacing;  // last!=0 -> get rid of last empty columns 
}
// ----------------------------------------------------------------
int ST7920_SPI::strWidth(char *str)
{
  int wd = 0;
  while (*str) wd += charWidth(*str++);
  return wd;
}

// ----------------------------------------------------------------
int ST7920_SPI::printCharInvert(int xpos, int ypos, unsigned char c, unsigned char cnext)
{
  if(xpos >= SCR_WD || ypos >= SCR_HT)  return 0;
  int fht8 = (cfont.ySize + 7) / 8, wd, fwd = cfont.xSize;
  int wd1, c1, cdata1;
  if(fwd < 0)  fwd = -fwd;

  c = convertPolish(c);

  if(c < cfont.firstCh || c > cfont.lastCh)  return c==' ' ?  1 + fwd/2 : 0;

  int x,y8,b,cdata = (c - cfont.firstCh) * (fwd*fht8+1) + 4;

  uint8_t d;


  wd = fontbyte(cdata++);

  int wdL = 0, wdR = spacing;
  if((*isNumberFun)(c)) {
    if(cfont.minDigitWd>wd) {
      wdL = (cfont.minDigitWd-wd)/2;
      wdR += (cfont.minDigitWd-wd-wdL);
    }
  } else
  if(cfont.minCharWd>wd) {
    wdL = (cfont.minCharWd-wd)/2;
    wdR += (cfont.minCharWd-wd-wdL);
  }
  if(xpos+wd+wdL+wdR>SCR_WD) wdR = max(SCR_WD-xpos-wdL-wd, 0);
  if(xpos+wd+wdL+wdR>SCR_WD) wd  = max(SCR_WD-xpos-wdL, 0);
  if(xpos+wd+wdL+wdR>SCR_WD) wdL = max(SCR_WD-xpos, 0);

  for(x=0; x<wd; x++) {
    uint8_t mask = 0x80 >> ((xpos+x+wdL)&7);
    for(y8=0; y8<fht8; y8++) {
      d = fontbyte(cdata+x*fht8+y8);
      int lastbit = cfont.ySize - y8 * 8;
      if (lastbit > 8) lastbit = 8;
      for(b=0; b<lastbit; b++) {
         if(d & 1)
        	 {
        	 //scr[(ypos+y8*8+b)*scrWd+(xpos+x+wdL)/8] = 0;
        	 	 scr[(ypos+y8*8+b)*scrWd+(xpos+x+wdL)/8] |= mask;  //drawPixel(xpos+x, ypos+y8*8+b, 1);
        	 }
         d>>=1;
      }
    }
  }

  if(cnext)
  {
    c1 = convertPolish(cnext);
    cdata1 = (c1 - cfont.firstCh) * (fwd*fht8+1) + 4;
    wd1 = fontbyte(cdata1);
  }
  else
  {
  	wd1 = wd;
  }
  return wd1+wdR+wdL;
}
// ----------------------------------------------------------------
int ST7920_SPI::printChar(int xpos, int ypos, unsigned char c)
{
  if(xpos >= SCR_WD || ypos >= SCR_HT)  return 0;
  int fht8 = (cfont.ySize + 7) / 8, wd, fwd = cfont.xSize;
  if(fwd < 0)  fwd = -fwd;

  c = convertPolish(c);
  if(c < cfont.firstCh || c > cfont.lastCh)  return c==' ' ?  1 + fwd/2 : 0;

  int x,y8,b,cdata = (c - cfont.firstCh) * (fwd*fht8+1) + 4;
  uint8_t d;
  wd = fontbyte(cdata++);
  int wdL = 0, wdR = spacing;
  if((*isNumberFun)(c)) {
    if(cfont.minDigitWd>wd) {
      wdL = (cfont.minDigitWd-wd)/2;
      wdR += (cfont.minDigitWd-wd-wdL);
    }
  } else
  if(cfont.minCharWd>wd) {
    wdL = (cfont.minCharWd-wd)/2;
    wdR += (cfont.minCharWd-wd-wdL);
  }
  if(xpos+wd+wdL+wdR>SCR_WD) wdR = max(SCR_WD-xpos-wdL-wd, 0);
  if(xpos+wd+wdL+wdR>SCR_WD) wd  = max(SCR_WD-xpos-wdL, 0);
  if(xpos+wd+wdL+wdR>SCR_WD) wdL = max(SCR_WD-xpos, 0);

  for(x=0; x<wd; x++) {
    uint8_t mask = 0x80 >> ((xpos+x+wdL)&7);
    for(y8=0; y8<fht8; y8++) {
      d = fontbyte(cdata+x*fht8+y8);
      int lastbit = cfont.ySize - y8 * 8;
      if (lastbit > 8) lastbit = 8;
      for(b=0; b<lastbit; b++) {
         if(d & 1)
        	 {
        	 //scr[(ypos+y8*8+b)*scrWd+(xpos+x+wdL)/8] = 0;
        	 	 scr[(ypos+y8*8+b)*scrWd+(xpos+x+wdL)/8] |= mask;  //drawPixel(xpos+x, ypos+y8*8+b, 1);
        	 }
         d>>=1;
      }
    }
  }
  return wd+wdR+wdL;
}
// ----------------------------------------------------------------
int ST7920_SPI::printStr(int xpos, int ypos, char *str, bool del)
{
//  unsigned char ch;
//  int stl, row;
  int x = xpos;
  int y = ypos;
  int wd = strWidth(str);
  if(xpos)
  {
  	xpos = xpos -1;
  }
  if(ypos)
  {
  	ypos = ypos -1;
  }
  if(del)
  {
  	fillRect(xpos,ypos , wd + 3 ,cfont.ySize + 2 ,0);
  }
  if(x==-1) // right = -1
    x = SCR_WD - wd;
  else if(x<0) // center = -2
    x = (SCR_WD - wd) / 2;
  if(x<0) x = 0; // left

  while(*str) {
    int wd1 = printChar(x,y,*str++);
    x+=wd1;
    if(cr && x>=SCR_WD) { 
      x=0; 
      y+=cfont.ySize; 
      if(y>SCR_HT) y = 0;
    }
  }
  return x;

}

int ST7920_SPI::printStrInvert(int xpos, int ypos, char *str, bool del)
{
	char _temp[50] = {0};
	strcpy(_temp, str);
	strrev(_temp);
//  unsigned char ch;
//  int stl, row;
  int x = xpos;
  int y = ypos;
  int count = 0;
  int wd = strWidth(str);
  if(xpos)
  {
  	xpos = xpos -1;
  }
  if(ypos)
  {
  	ypos = ypos -1;
  }
  if(del)
  {
  	deleteString(xpos, ypos, 4, 0);
  }
  if(x==-1) // right = -1
    x = SCR_WD - wd;
  else if(x<0) // center = -2
    x = (SCR_WD - wd) / 2;
  if(x<0) x = 0; // left

  while(_temp[count]) {
    int wd1 = printCharInvert(x,y,_temp[count], _temp[count+1]);
//    if(*(str) == '1')
//    	wd1 = 5;
//    if(*(str+1) == '1' )
//    	wd1 = 4;
    count++;
    x-=wd1;
    if(cr && x>=SCR_WD) {
      x=0;
      y+=cfont.ySize;
      if(y>SCR_HT) y = 0;
    }
  }
  return x;

}

bool ST7920_SPI::isdigit(uint8_t ch)
{
	if(ch >= 0x30 && ch <= 0x39)
		return true;
	else
		return false;
}
bool ST7920_SPI::isNumber(uint8_t ch)
{
  return isdigit(ch) || ch==' ';
}
// ---------------------------------
bool ST7920_SPI::isNumberExt(uint8_t ch)
{
  return isdigit(ch) || ch=='-' || ch=='+' || ch=='.' || ch==' ';
}
// ----------------------------------------------------------------
unsigned char ST7920_SPI::convertPolish(unsigned char _c)
{
  unsigned char pl, c = _c;
  if(c==196 || c==197 || c==195) {
	  dualChar = c;
    return 0;
  }
  if(dualChar) { // UTF8 coding
    switch(_c) {
      case 133: pl = 1+9; break; // '�'
      case 135: pl = 2+9; break; // '�'
      case 153: pl = 3+9; break; // '�'
      case 130: pl = 4+9; break; // '�'
      case 132: pl = dualChar==197 ? 5+9 : 1; break; // '�' and '�'
      case 179: pl = 6+9; break; // '�'
      case 155: pl = 7+9; break; // '�'
      case 186: pl = 8+9; break; // '�'
      case 188: pl = 9+9; break; // '�'
      //case 132: pl = 1; break; // '�'
      case 134: pl = 2; break; // '�'
      case 152: pl = 3; break; // '�'
      case 129: pl = 4; break; // '�'
      case 131: pl = 5; break; // '�'
      case 147: pl = 6; break; // '�'
      case 154: pl = 7; break; // '�'
      case 185: pl = 8; break; // '�'
      case 187: pl = 9; break; // '�'
      default:  return c; break;
    }
    dualChar = 0;
  } else   
  switch(_c) {  // Windows coding
    case 165: pl = 1; break; // �
    case 198: pl = 2; break; // �
    case 202: pl = 3; break; // �
    case 163: pl = 4; break; // �
    case 209: pl = 5; break; // �
    case 211: pl = 6; break; // �
    case 140: pl = 7; break; // �
    case 143: pl = 8; break; // �
    case 175: pl = 9; break; // �
    case 185: pl = 10; break; // �
    case 230: pl = 11; break; // �
    case 234: pl = 12; break; // �
    case 179: pl = 13; break; // �
    case 241: pl = 14; break; // �
    case 243: pl = 15; break; // �
    case 156: pl = 16; break; // �
    case 159: pl = 17; break; // �
    case 191: pl = 18; break; // �
    default:  return c; break;
  }
  return pl+'~'+1;
}

void ST7920_SPI::drawProgressBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t percent)
{
	fillRect(x, y, w, h, 0);
	drawRect(x, y, w, h, 1);
	float _value = percent * (w - 2) / 100.0f;
	fillRect(x + 1, y + 1, (uint8_t)_value, h-2, 1);
}

void ST7920_SPI::invertString(uint8_t x, uint8_t y, uint8_t length, uint8_t offsetLength)
{
	uint8_t _wide = length*(-cfont.xSize) + offsetLength + 4;
	uint8_t _fontSize = -cfont.xSize;
	uint8_t _xpos = x + _fontSize - _wide;
	fillRect(_xpos,y-1,_wide + 2 ,cfont.ySize + 1,2);
}

void ST7920_SPI::deleteString(uint8_t x, uint8_t y, uint8_t length, uint8_t offsetLength)
{
	uint8_t _wide = length*(-cfont.xSize) + offsetLength + 4;
	uint8_t _fontSize = -cfont.xSize;
	uint8_t _xpos = x + _fontSize - _wide;
	fillRect(_xpos,y-1,_wide + 2 ,cfont.ySize + 2,0);
}
// ---------------------------------
