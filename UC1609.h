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
#ifndef _UC1609_H
#define _UC1609_H

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SPIDevice.h>
#include <SPI.h>

#define BLACK 1 ///< Black pixel
#define WHITE 0 ///< White pixel

#define LCDWIDTH 192  ///< LCD is 192 pixels wide
#define LCDHEIGHT 64 ///< 64 pixels high

// UC1909 Write registers
#define UC1609_SYSTEM_RESET 0xE2 /**< System Reset */

#define UC1609_POWER_CONTROL 0x28 /**< Power control Address */
#define UC1609_PC_SET 0x06 /**< PC[2:0] 110, Internal V LCD (7x charge pump) + 10b: 1.4mA */

#define UC1609_ADDRESS_CONTROL 0x88 /**< set RAM address control */
#define UC1609_ADDRESS_SET 0x02 /**< Set AC [2:0] Program registers  for RAM address control.*/

#define UC1609_SET_PAGEADD 0xB0 /**< Page address Set PA[3:0]  */
#define UC1609_SET_COLADD_LSB 0x00 /**< Column Address Set CA [3:0] */
#define UC1609_SET_COLADD_MSB 0x10 /**< Column Address Set CA [7:4] */

#define UC1609_TEMP_COMP_REG 0x27 /**< Temperature Compensation Register */
#define UC1609_TEMP_COMP_SET 0x00 /**< TC[1:0] = 00b= -0.00%/ C */

#define UC1609_FRAMERATE_REG 0xA0 /**< Frame rate register */
#define UC1609_FRAMERATE_SET 0x01  /**< Set Frame Rate LC [4:3] 01b: 95 fps */

#define UC1609_BIAS_RATIO 0xE8 /**< Bias Ratio. The ratio between V-LCD and V-D . */
#define UC1609_BIAS_RATIO_SET 0x03 /**<  Set BR[1:0] = 11 (set to 9 default, 11b = 9) */

#define UC1609_GN_PM 0x81 /**< Set V BIAS Potentiometer to fine tune V-D and V-LCD  (double-byte command) */
#define UC1609_DEFAULT_GN_PM 0x49 /**< default only used if user does not specify Vbias */

#define UC1609_LCD_CONTROL 0xC0 /**< Rotate map control */
#define UC1609_DISPLAY_ON 0xAE /**< enables display */
#define UC1609_ALL_PIXEL_ON 0xA4 /**< sets on all Pixels on */
#define UC1609_INVERSE_DISPLAY 0xA6 /**< inverts display */
#define UC1609_SCROLL 0x40 /**< scrolls , Set the scroll line number. 0-64 */

// Rotate
#define UC1609_ROTATION_FLIP_TWO 0x06
#define UC1609_ROTATION_NORMAL 0x04
#define UC1609_ROTATION_FLIP_ONE 0x02
#define UC1609_ROTATION_FLIP_THREE 0x00

/**************************************************************************/
/*!
    @brief The UC1609 class
 */
class UC1609 : public Adafruit_GFX {
public:
  UC1609(int8_t sclk_pin, int8_t din_pin, int8_t dc_pin,
                   int8_t cs_pin, int8_t rst_pin);
  UC1609(int8_t dc_pin, int8_t cs_pin, int8_t rst_pin,
                   SPIClass *theSPI = &SPI);
  ~UC1609(void);

  bool begin();
  void command(uint8_t c);
  void data(uint8_t c);
 
  void setContrast(uint8_t val);
  uint8_t getContrast(void);
  
  void display(void);
  void clearDisplay(void);

  void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax,
                         uint8_t ymax);
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  void initDisplay();
  void invertDisplay(bool i);

private:
  Adafruit_SPIDevice *spi_dev = NULL;
  uint8_t *buffer; ///< Buffer data used for display buffer. Allocated when
                   ///< begin method is called.

  int8_t _rstpin = -1, _dcpin = -1;

  uint8_t _VbiasPot;        ///< Contrast level, Vop
  uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
};

#endif
