# UC1609 library
This is a library for UC1609 Monochrome LCD Displays. These displays use I2C or SPI to communicate, 2 to 5 pins are required to interface.
This library depends on Adafruit GFX library at https://github.com/adafruit/Adafruit-GFX-Library. Please make sure you have installed the latest version before using this library.

This is a free library WITH NO WARRANTY, use it at your own risk!


![Image](https://github.com/nstepanets/UC1609/blob/d6e61c78ad12e1b4f57803de1b8746e4804b48e0/images/UC1609_display.jpg)

# UC1609 interface
UC1609c supports three serial modes, one 4-wire SPI, one 3-wire SPI and one 2-wire I2C mode. Bus interface mode is determined by the wiring of the BM[1:0] and D[7] pins.

![Display connection](https://github.com/nstepanets/UC1609/blob/d6e61c78ad12e1b4f57803de1b8746e4804b48e0/images/UC1609_interface.png)

# I2C (2-wire) interface
When BM0 and D7 are set to "H", UC1609c is configured as an I2C bus signaling protocol compliant device. In this mode, CS pin is used to configure UC1609c device address (0x3C or 0x3E).

CD pin is not used in I2C mode and should be connected to Vss

When RST pin is not used (since RST pin is not required for proper chip operation), connect the pin to "H"

![I2C mod](https://github.com/nstepanets/UC1609/blob/d6e61c78ad12e1b4f57803de1b8746e4804b48e0/images/UC1609_i2c_mod.jpg)

To select I2C mode, SPI displays require a little modification - isolate Pin 9 from Vss and connect Pin 9 and Pin 15 to Vdd.
