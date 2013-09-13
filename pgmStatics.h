#if !defined(__PGMSTATICS_H__)
#define __PGMSTATICS_H__

// Thermostat Default Desired Temp
static double DESIRED_TEMP=72.0;

// USBHost Reset Pin
static uint8_t USB_RST_dPIN=7;

// TMP36 GLOBALS
static uint8_t TMP36_aPIN=0;                //the resolution is 10 mV / degree centigrade 
//(500 mV offset) to make negative temperatures an option
static double aref_voltage_33=3.3;         // we tie 3.3V to ARef and measure it with a multimeter!
//double aref_voltage_50=5.0;         // we tie 5.0V to ARef and measure it with a multimeter!

static uint8_t LCD_BL_EN_PIN=A5;

// make some custom characters for LCD LiquidCrystal
static byte degree[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
static uint8_t degreeChar = 0;

static byte smiley[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};
static uint8_t smileyChar = 1;

static byte frownie[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001
};
static uint8_t frownieChar = 2;

static byte relayON[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};
static uint8_t relayONChar = 3;

static byte relayOFF[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100
};
static uint8_t relayOFFChar = 4;

#endif // __PGMSTATICS_H__
