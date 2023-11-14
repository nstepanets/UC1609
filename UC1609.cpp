/**********************************************************************************
* This is a library for UC1609 Monochrome LCD Display.
* These displays use SPI to communicate, 4 or 5 pins are required to  
*   interface
* This is a free library WITH NO WARRANTY, use it at your own risk!
***********************************************************************************
* This library depends on Adafruit GFX library at
*   https://github.com/adafruit/Adafruit-GFX-Library
*   being present on your system. Please make sure you have installed the latest
*   version before using this library.
***********************************************************************************/

#include "UC1609.h"
#include "Arduino.h"
#include <stdlib.h>

/*!
  @brief Constructor for software SPI with explicit CS pin
  @param sclk_pin SCLK pin
  @param din_pin  DIN pin
  @param dc_pin   DC pin
  @param cs_pin   CS pin
  @param rst_pin  RST pin
 */
UC1609::UC1609(int8_t sclk_pin, int8_t din_pin,
               int8_t dc_pin, int8_t cs_pin, int8_t rst_pin)
    : Adafruit_GFX(LCDWIDTH, LCDHEIGHT) {
  spi_dev = new Adafruit_SPIDevice(cs_pin, sclk_pin, -1, din_pin,
                                   8000000); // 8mhz max speed

  _dcpin = dc_pin;
  _rstpin = rst_pin;
}

/*!
  @brief Constructor for hardware SPI based on hardware controlled SCK (SCLK)
  and MOSI (DIN) pins. CS is still controlled by any IO pin. NOTE: MISO and SS
  will be set as an input and output respectively, so be careful sharing those
  pins!
  @param dc_pin   DC pin
  @param cs_pin   CS pin
  @param rst_pin  RST pin
  @param theSPI   Pointer to SPIClass device for hardware SPI
 */
UC1609::UC1609(int8_t dc_pin, int8_t cs_pin, int8_t rst_pin,
               SPIClass *theSPI)
    : Adafruit_GFX(LCDWIDTH, LCDHEIGHT) {
  spi_dev = new Adafruit_SPIDevice(cs_pin, 8000000, SPI_BITORDER_MSBFIRST,
                                   SPI_MODE0, theSPI);

  _dcpin = dc_pin;
  _rstpin = rst_pin;
}

/*!
    @brief  Destructor for UC1609 object.
*/
UC1609::~UC1609(void) {
  if (buffer) {
    free(buffer);
    buffer = NULL;
  }
}

/*!
  @brief  Send a command to the LCD
  @param c Command byte
 */
void UC1609::command(uint8_t c) {
  digitalWrite(_dcpin, LOW);
  spi_dev->write(&c, 1);
}

/*!
  @brief  Send data to the LCD
  @param c Data byte
 */
void UC1609::data(uint8_t c) {
  digitalWrite(_dcpin, HIGH);
  spi_dev->write(&c, 1);
}

/*!
  @brief Update the bounding box for partial updates
  @param xmin left
  @param ymin top
  @param xmax right
  @param ymax bottom
 */
void UC1609::updateBoundingBox(uint8_t xmin, uint8_t ymin,
                                         uint8_t xmax, uint8_t ymax) {
  xUpdateMin = min(xUpdateMin, xmin);
  xUpdateMax = max(xUpdateMax, xmax);
  yUpdateMin = min(yUpdateMin, ymin);
  yUpdateMax = max(yUpdateMax, ymax);
}

/*!
  @brief The most basic function, set a single pixel, in the main buffer
  @param x     x coord
  @param y     y coord
  @param color pixel color (BLACK or WHITE)
 */
void UC1609::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
    return;

  int16_t t;
	switch (rotation) {
  case 1:
    t = x;
    x = y;
    y = LCDHEIGHT - 1 - t;
    break;
  case 2:
    x = LCDWIDTH - 1 - x;
    y = LCDHEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = LCDWIDTH - 1 - y;
    y = t;
    break;
  }
  updateBoundingBox(x, y, x, y);
	
  // x is which column
  if (color)
    buffer[x + (y / 8) * LCDWIDTH] |= 1 << (y % 8);
  else
    buffer[x + (y / 8) * LCDWIDTH] &= ~(1 << (y % 8));
}

/*!
  @brief Clear the entire display
 */
void UC1609::clearDisplay(void) {
  memset(buffer, 0, WIDTH * ((HEIGHT + 7) / 8));
  updateBoundingBox(0, 0, LCDWIDTH - 1, LCDHEIGHT - 1);
  cursor_y = cursor_x = 0;
}

/*!
  @brief Initialize the display. Set bias and contrast, enter normal mode.
 */
void UC1609::initDisplay() {

  // toggle RST low to reset
  if (_rstpin >= 0) {
    pinMode(_rstpin, OUTPUT);
    digitalWrite(_rstpin, LOW);
    delay(1); // 1 ns minimum
    digitalWrite(_rstpin, HIGH);
    delay(5);
  }
  
  command(UC1609_SET_COM_END); /* set COM end (display height-1) */
  command(LCDHEIGHT - 1);
  command(UC1609_TEMP_COMP_REG | UC1609_TEMP_COMP_SET); /* set temp. compensation */
  command(UC1609_LCD_CONTROL | UC1609_ROTATION_DEFAULT); /* SEG & COM normal */
  command(UC1609_SCROLL | 0); /* set scroll line to zero */
  command(UC1609_POWER_CONTROL | UC1609_PC_SET); /* chare pump */
  command(UC1609_BIAS_RATIO | UC1609_BIAS_RATIO_SET); /* set bias 1/2 */
  command(UC1609_GN_PM); /* set contrast */
  command(UC1609_DEFAULT_GN_PM);

  /*
    AC0:	0: stop at boundary, 1: increment by one
    AC1: 	0: first column then page, 1: first page, then column increment
    AC2:	0: increment page adr, 1: decrement page adr.
  */
  command(UC1609_ADDRESS_CONTROL | UC1609_ADDRESS_SET); /* set auto increment, low bits are AC2 AC1 AC0 */
  command(UC1609_FRAMERATE_REG | UC1609_FRAMERATE_SET); /* frame rate 95Hz */

  /*
    LC0:	0
    MX:	Mirror X
    MY:	Mirror Y
  */  
  command(UC1609_LCD_CONTROL | UC1609_ROTATION_NORMAL); /* low bits are MY, MX, LC0 */
  command(UC1609_INVERSE_DISPLAY | 0); /* set normal pixel mode (not inverse) */
  command(UC1609_ALL_PIXEL_ON | 0); /* set normal pixel mode (not all on) */

}
 
/*!
  @brief Set up SPI, initialize the display
  @returns True on initialization success
 */
bool UC1609::begin() {

  if ((!buffer) && !(buffer = (uint8_t *)malloc(WIDTH * ((HEIGHT + 7) / 8))))
    return false;

  if (!spi_dev->begin()) {
    return false;
  }

  // Set common pin outputs.
  pinMode(_dcpin, OUTPUT);
  if (_rstpin >= 0)
    pinMode(_rstpin, OUTPUT);

  initDisplay();
  // clear display and set up a bounding box for screen updates
  clearDisplay();
  // update the display
  display();
  // display on
  command(UC1609_DISPLAY_ON | 1);
  
  return true;
}

/*!
  @brief  Set the contrast level
  @param val Contrast value
 */
void UC1609::setContrast(uint8_t VbiasPot) {
  _VbiasPot = VbiasPot;
  command(UC1609_GN_PM);
  command(_VbiasPot);
}

/*!
  @brief  Get the contrast level
  @return Contrast value
 */
uint8_t UC1609::getContrast() { return _VbiasPot; }

/*!
  @brief Update the display
 */
void UC1609::display(void) {

  for(uint16_t p = yUpdateMin / 8; p <= yUpdateMax / 8; p++) {
    command( UC1609_SET_PAGEADD | p );

    // start at the beginning of the row
    command(UC1609_SET_COLADD_LSB | (xUpdateMin & 0x0F) );
    command(UC1609_SET_COLADD_MSB | ((xUpdateMin & 0xF0) >> 4) );

    for(uint8_t col = xUpdateMin; col <= xUpdateMax; col++) {
      data(buffer[ (p * LCDWIDTH) + col ]);
    }
  }

  xUpdateMin = LCDWIDTH - 1;
  xUpdateMax = 0;
  yUpdateMin = LCDHEIGHT - 1;
  yUpdateMax = 0;
}

/*!
  @brief Invert the entire display
  @param i True to invert the display, false to keep it uninverted
 */
void UC1609::invertDisplay(bool i) {
  command(UC1609_INVERSE_DISPLAY | i);
}
