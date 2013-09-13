/*
* USB HOST SHIELD PINS
   D7 - D13
   RES
   VIN
   GND
* TMP36 PINS
   A0
   3.3V
   AREF
   GND
* LCD PINS:
   D2-D6
   A1
*/
/*
 Relays on PIC board:
 1 - White (HEATING)
 2 - Yellow (COOLING)
 3 - Green (FAN)
 */

#include <avrpins.h>
#include <max3421e.h>
#include <usbhost.h>
#include <usb_ch9.h>
#include <Usb.h>
#include <usbhub.h>
#include <avr/pgmspace.h>
#include <address.h>
#include <cdcftdi.h>
#include <printhex.h>
#include <message.h>
#include <hexdump.h>
#include <parsetools.h>

#include <LiquidCrystal.h>
#include "FTDIAsync.h"

#define DEBUG_USB_HOST 0

// USBHost globals
USB              Usb;
//USBHub         Hub(&Usb);
FTDIAsync        FtdiAsync;
FTDI             Ftdi(&Usb, &FtdiAsync);
// USBHost Reset Pin
#define USB_RST_dPIN 7

// TMP36 GLOBALS
#define TMP36_aPIN 0                //the resolution is 10 mV / degree centigrade 
//(500 mV offset) to make negative temperatures an option
#define aref_voltage_33 3.3         // we tie 3.3V to ARef and measure it with a multimeter!
#define aref_voltage_50 5.0         // we tie 3.3V to ARef and measure it with a multimeter!

double degreeCorrectionF = 0.0;
double DESIRED_TEMP=72.0;
boolean tooHot= false, wayHot = false, tooCold = false, wayCold = false, justRight=false;
float tempF=0, tempC=0;
unsigned long uptime = 0;

enum THERMOSTAT_STATES {
  OFF, COOL, HEAT, FAN};

THERMOSTAT_STATES last_thermostat_state = OFF, curr_thermostat_state = OFF, user_thermostat_state = OFF;

boolean relays[16];
boolean haveRestored = false, userChanged = false;

uint8_t intbuffer2[ 12 ] = { 
  0x00         };
uint8_t buf[ 64 ] = { 
  0x00                 }; //buffer to convert unsigned long to ASCII, and to hold uptime
uint8_t  rcode = 0;

// LCD GLOBALS
char strbuf[6]; // holds the ascii for the command
char intbuffer[2]; // holds the ascii for an integer

enum LCD_STATES { 
  LCD_OFF, LCD_ON, CLEAR, WELCOME, UPTIME, TEMP, STATUS, CUSTOM };

LCD_STATES curr_lcd_state = STATUS, last_lcd_state = LCD_OFF;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(A1, 2, 3, 4, 5, 6);
#define LCD_BL_EN_PIN A5

// make some custom characters:
byte degree[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
uint8_t degreeChar = 0;

byte smiley[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};
uint8_t smileyChar = 1;

byte frownie[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001
};
uint8_t frownieChar = 2;

byte relayON[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};
uint8_t relayONChar = 3;

byte relayOFF[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100
};
uint8_t relayOFFChar = 4;

void setup()
{
  // set up USB host by bringing it out of reset state
  pinMode(USB_RST_dPIN, OUTPUT);
  digitalWrite(USB_RST_dPIN, HIGH);

  // set up TMP36
  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);
  pinMode(TMP36_aPIN, INPUT);

  // set up the LCD's number of columns and rows and turn backlight OFF
  pinMode(LCD_BL_EN_PIN, OUTPUT);
  digitalWrite(LCD_BL_EN_PIN, LOW);  
  lcd.begin(16, 2);
  // create a new character
  lcd.createChar(degreeChar, degree);
  // create a new character
  lcd.createChar(smileyChar, smiley);
  // create a new character
  lcd.createChar(frownieChar, frownie);
  lcd.createChar(relayONChar, relayON);
  lcd.createChar(relayOFFChar, relayOFF);

  // set up Serial
  Serial.begin( 115200 );  

  if (Usb.Init() == -1){
    Serial.println("OSC did not start.");
    while(1); //halt
  }//if (Usb.Init() == -1...

  delay( 200 );

  Serial.println("Started! Type '?' for help.");
}

void loop()
{
  Usb.Task();

  // Process any incoming Serial messages
  uint8_t userCommand[64] = { 
    0x00                            }; //buffer to hold the message coming in from Serial
  int len=0;
  while (Serial.available() && len<64) {
    // read in one character at a time
    userCommand[len] = (char)Serial.read();
    len++;
    delay(1);
  }

  if(len > 0) {    
    for( uint8_t l = 0; l < len; l++ ) {
      Serial.print((char)userCommand[l]);
    }
    Serial.println("");

    if(len == 1) { //one character was sent {char,\n}


      // execute command that was called
      switch((char)userCommand[0]) {
      case 'u': // send out arduino uptime
        // write to Serial
        Serial.print("Uptime: ");
        ultoa( millis()/1000, (char *)intbuffer, 10 );
        Serial.print((char *)intbuffer);
        Serial.println(" seconds elapsed");
        // write to LCD
        printUptimetoLCD((char *)intbuffer);
        // set up refresh
        curr_lcd_state = UPTIME;
        break;
      case 't': // get temperature from TMP36 sensor
        // write to Serial
        itoa((int)tempF, (char *)buf, 10);
        strcat((char *)buf, ".");
        itoa(abs((tempF - (int)tempF)*100), (char *)intbuffer, 10);
        strcat((char *)buf, (char *)intbuffer);
        strcat((char *)buf, " F");
        Serial.println((char *)buf);
        // write to LCD
        printTemptoLCD();
        // set up refresh
        curr_lcd_state = TEMP;
        break;
      case 'H':
        user_thermostat_state = HEAT;
        break;
      case 'C':
        user_thermostat_state = COOL;
        break;
      case 'F':
        user_thermostat_state = FAN;
        break;
      case 'O':
        user_thermostat_state = OFF;
        break;
      case 'S':
        Serial.println("Relay Statuses:\r\n---------");
        for(int o = 0; o<16; o++) {
          Serial.print(o+1);
          Serial.print(": ");
          Serial.println(relays[o]);
        }
        Serial.println("=========");
        Serial.println("States:\r\n---------");
        Serial.print("curr: ");
        Serial.println(curr_thermostat_state);
        Serial.print("last: ");
        Serial.println(last_thermostat_state);
        Serial.print("user: ");
        Serial.println(user_thermostat_state);
        Serial.print("tooHot: ");
        Serial.println(tooHot);
        Serial.print("wayHot: ");
        Serial.println(wayHot);
        Serial.print("tooCold: ");
        Serial.println(tooCold);
        Serial.print("wayCold: ");
        Serial.println(wayCold);
        Serial.print("Desired Temp: ");
        Serial.println(DESIRED_TEMP + degreeCorrectionF);
        Serial.print("userChanged: ");
        Serial.println(userChanged);
        Serial.print("haveRestored: ");
        Serial.println(haveRestored);
        Serial.println("=========");
        curr_lcd_state = STATUS;
        break;
      case '+':      
        degreeCorrectionF += 0.5;
        Serial.print("Desired Temp: ");
        Serial.println(DESIRED_TEMP + degreeCorrectionF);
        break;
      case '-':
        degreeCorrectionF -= 0.5;
        Serial.print("Desired Temp: ");
        Serial.println(DESIRED_TEMP + degreeCorrectionF);
        break;
      case '?': // help menu
        Serial.println("Welcome to Help Menu for Arduino Thermostat");        
        Serial.println("\t'?': This Help Menu");
        Serial.println("\t'w': Show Welcome on LCD");
        Serial.println("\t'l': Toggle LCD On/Off");        
        Serial.println("\t'u': Arduino's Uptime on LCD");
        Serial.println("\t't': Get current Temperature on LCD");        
        Serial.println("\t'S': Get current HVAC status");
        Serial.println("\t'+': Increase Desired Temperature by 0.5");
        Serial.println("\t'-': Decrease Desired Temperature by 0.5");
        Serial.println("\t'H': Turn the HEATER on (relays 1 and 3 on)");
        Serial.println("\t'C': Turn the AC on (relays 2 and 3 on)");
        Serial.println("\t'F': Turn the FAN on (relay 3 on)");
        Serial.println("\t'O': Turn the HVAC off (all relays off)");
        break;
      case 'w':
        Serial.println("Showing Welcome Banner");
        curr_lcd_state = WELCOME;
        break;
      case 'l':
        if(curr_lcd_state == LCD_OFF) {
          curr_lcd_state = LCD_ON;
        } 
        else {
          curr_lcd_state = LCD_OFF;
        }        
        break;
      default:
        break;
      }
    }
    else if (len >= 2) { // if message received is larger than 2, it's a custom message
      lcd.clear();
      for( uint8_t l = 0; l < len; l++ ) {
        lcd.print((char)userCommand[l]);
      }
      curr_lcd_state = CUSTOM;
    } //end if len == 1
  } // end if len > 0

  refreshScreen();
} // end of void loop

void turnLCDon() {
  if(digitalRead(LCD_BL_EN_PIN) == LOW) {
    Serial.println("turning on LCD...");
    lcd.display();
    digitalWrite(LCD_BL_EN_PIN, HIGH);
    //Serial.println("done!");
  }
}

void turnLCDoff() {
  if(digitalRead(LCD_BL_EN_PIN) == HIGH) {
    Serial.println("turning off LCD...");
    lcd.noDisplay();
    digitalWrite(LCD_BL_EN_PIN, LOW);
    //Serial.println("done!");
  }
}

void refreshScreen() {
  uptime = millis();

  if(uptime % 1000 == 0) { // every second after the first second

    float temperature = getVoltage(TMP36_aPIN);

    tempF = (((temperature - 0.5) * 100.0) * (9.0 / 5.0)) + 32.0;          //converting from 10 mv per degree with 500 mV offset
    tempC = (tempF - 32.0) * (5.0 / 9.0);

    // Refresh LCD
    switch(curr_lcd_state) {
    case CLEAR:
      if(curr_lcd_state != last_lcd_state) {
        lcd.clear();
      }
      break;
    case WELCOME:
      if(curr_lcd_state != last_lcd_state) {
        turnLCDon();        
        lcd.clear();
        lcd.print("Welcome To:");
        lcd.setCursor(0,2);
        lcd.print("ADK_term_test v2");
      }
      break;
    case UPTIME:
      if(curr_lcd_state != last_lcd_state) {
        turnLCDon();
      }
      ultoa( uptime/1000, (char *)intbuffer2, 10 ); 
      printUptimetoLCD((char *)intbuffer2);
      break;
    case TEMP:
      if(curr_lcd_state != last_lcd_state) {
        turnLCDon();
      }
      printTemptoLCD();
      break;
    case LCD_ON:
      if(curr_lcd_state != last_lcd_state) {
        turnLCDon();
      }
      break;
    case LCD_OFF:
      if(curr_lcd_state != last_lcd_state) {
        turnLCDoff();
      }
      break;
    case STATUS:
      if(curr_lcd_state != last_lcd_state) {
        turnLCDon();
      }
      printStatustoLCD();
      break;
    case CUSTOM:
      if(curr_lcd_state != last_lcd_state) {
        turnLCDon();
      }
      break;
    }
    last_lcd_state = curr_lcd_state;

    // if USB FTDI is connected
    if( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {

      for(int l=0;l<6;l++) {
        strbuf[l] = 0;
      }

      //  set relays accordingly
      // get all the relays' statuses
      TxRxFTDI("ask//");

      // sanity check... if any unknown relays are turned on... turn them all off
      boolean resetRelays = false;
      for(int p = 3; p<16; p++) {
        if(relays[p]) {
          resetRelays = true;
        }
      }
      if(resetRelays) {
        TxRxFTDI("off//");
      }

      // sanity check in case the Arduino was reset or unplugged from PIC and the current relay state is to be restored
      if(relays[2] && !relays[0] && !relays[1]) last_thermostat_state = FAN;
      else if(relays[1] && !relays[0]) last_thermostat_state = COOL;
      else if(relays[0] && !relays[1]) last_thermostat_state = HEAT;
      else last_thermostat_state = OFF;

      // make sure we do a sanity check... this should only run once first time through loop() after first succesful USB_STATE_RUNNING
      if(!haveRestored) {
        curr_thermostat_state = last_thermostat_state;
        user_thermostat_state = last_thermostat_state;
        haveRestored = true;
      }

      if(curr_thermostat_state != user_thermostat_state && !userChanged && last_thermostat_state != OFF) {
        curr_thermostat_state = user_thermostat_state;
        userChanged = true;
      }

      tooHot = (tempF >= ((DESIRED_TEMP + degreeCorrectionF) + 1.0));
      wayHot = (tempF >= ((DESIRED_TEMP + degreeCorrectionF) + 10.0));
      tooCold = (tempF <= ((DESIRED_TEMP + degreeCorrectionF) - 1.0));
      wayCold = (tempF <= ((DESIRED_TEMP + degreeCorrectionF) - 10.0));
      justRight = ((tempF <= (DESIRED_TEMP + degreeCorrectionF) + 1.0) && tempF >= ((DESIRED_TEMP + degreeCorrectionF) - 1.0));

      if(curr_thermostat_state != last_thermostat_state || (tooHot || tooCold || justRight || userChanged)) {
        Serial.print("...");
        Serial.print(last_thermostat_state);
        Serial.print(curr_thermostat_state);
        Serial.print(user_thermostat_state);
        Serial.println(userChanged);
        switch(curr_thermostat_state) {
        case COOL:
          // turn AC on if current mode is COOL and we weren't already in COOL mode and it's tooHot
          //Serial.println("COOL?");
          if(curr_thermostat_state != last_thermostat_state && (tooHot || wayHot)) {
            Serial.println("COOL!");
            if(relays[0]) TxRxFTDI("01-//");
            if(!relays[1]) TxRxFTDI("02+//");
            if(!relays[2]) TxRxFTDI("03+//");
          }
          // turn mode to OFF if we were cooling and it's tooCold(implied) or userChanged(implied)
          else if(last_thermostat_state != OFF && curr_thermostat_state != last_thermostat_state) {
            //Serial.println(".");
            curr_thermostat_state = OFF;
            TxRxFTDI("off//");
            return;
          }
          break;
        case HEAT:
          // turn HEATER on if current mode is HEAT and we weren't already in HEAT mode and it's tooCold
          //Serial.println("HEAT?");
          if (curr_thermostat_state != last_thermostat_state && (tooCold || wayCold)){                        
            Serial.println("HEAT!");
            if(relays[1]) TxRxFTDI("02-//");
            if(!relays[0]) TxRxFTDI("01+//");            
            if(!relays[2]) TxRxFTDI("03+//");
          }
          // turn mode to OFF if we were heating and it's tooHot(implied) or userChanged(implied)
          else if(last_thermostat_state != OFF) {
            //Serial.println("HEAT<");
            curr_thermostat_state = OFF;
            TxRxFTDI("off//");
            return;
          }
          break;
        case FAN:
          // turn FAN on if current mode is FAN and we weren't already in FAN mode
          //Serial.println("FAN?");
          if(curr_thermostat_state != last_thermostat_state) {
            Serial.println("FAN!");
            if(relays[0]) TxRxFTDI("01-//");
            if(relays[1]) TxRxFTDI("02-//");
            if(!relays[2]) TxRxFTDI("03+//");
          }
          break;
        case OFF:
          // turn OFF if current mode is OFF and we weren't already in OFF mode
          //Serial.println("OFF?");
          if(curr_thermostat_state != last_thermostat_state || userChanged) {
            Serial.println("OFF!");
            if(relays[0]) TxRxFTDI("01-//");
            if(relays[1]) TxRxFTDI("02-//");
            if(relays[2]) TxRxFTDI("03-//");
          }
          break;
        }
        last_thermostat_state = curr_thermostat_state;        
        userChanged = false;
      } // end if(curr_thermostat_state != last_thermostat_state)

      // logic to automatically switch on AC or HEAT in case system is currently OFF
      // AUTOMATICALLY turn on COOL if  tooHot and user had selected COOL, or wayHot and we were OFF
      if((tooHot && user_thermostat_state == COOL)  || (wayHot && user_thermostat_state == OFF)) {
        curr_thermostat_state = COOL;
      }
      // AUTOMATICALLY turn on HEAT if tooCold and user had selected HEAT, or wayCold and we were OFF
      if ((tooCold && user_thermostat_state == HEAT) || (wayCold && user_thermostat_state == OFF)) {
        //user_thermostat_state = HEAT;
        curr_thermostat_state = HEAT;
      }
      // AUTOMATICALLY turn on FAN if user had selected FAN
      if(user_thermostat_state == FAN) {
        curr_thermostat_state = FAN;
      }
      // AUTOMATICALLY turn OFF if user had selected OFF
      if(user_thermostat_state == OFF) {
        curr_thermostat_state = OFF;
      }      
    } // end if( Usb.getUsbTaskState() == USB_STATE_RUNNING )
    else {
      // reset our global for next USB connection
      haveRestored = false;
    }
  } // if uptime is divisible by 1000 millis, update the screen
}

void TxRxFTDI(char strbuf[]) {
  //char strbuf[] = "off//";
  //Serial.print("Sending: ");
  //Serial.println(strbuf);
  rcode = Ftdi.SndData(strlen(strbuf), (uint8_t*)strbuf);
  if (rcode) {
    //Serial.println("Error with SndData!");
    ErrorMessage<uint8_t>(PSTR("SndData"), rcode);
  }

  delay(15);

  uint16_t rcvd = 64;
  rcode = Ftdi.RcvData(&rcvd, buf);

  if (rcode && rcode != hrNAK) {
    //Serial.println("Error with RcvData!");
    ErrorMessage<uint8_t>(PSTR("Ret"), rcode);
  }

  // The device reserves the first two bytes of data
  //   to contain the current values of the modem and line status registers.
  if (rcvd > 2) {
    //Serial.print("Receiving: ");
    //Serial.println((char*)(buf+2));
  }
  delay(15);

  if(strbuf == "ask//") {
    //Serial.println("received relay status!");
    char* buffer = ((char*)(buf+2));
    /*Serial.println((buffer[0] & 0x01) >> 0);
     Serial.println((buffer[0] & 0x02) >> 1);
     Serial.println((buffer[0] & 0x04) >> 2);
     Serial.println((buffer[0] & 0x08) >> 3);
     Serial.println((buffer[0] & 0x10) >> 4);
     Serial.println((buffer[0] & 0x20) >> 5);
     Serial.println((buffer[0] & 0x40) >> 6);
     Serial.println((buffer[0] & 0x80) >> 7);
     Serial.println((buffer[1] & 0x01) >> 0);
     Serial.println((buffer[1] & 0x02) >> 1);
     Serial.println((buffer[1] & 0x04) >> 2);
     Serial.println((buffer[1] & 0x08) >> 3);
     Serial.println((buffer[1] & 0x10) >> 4);
     Serial.println((buffer[1] & 0x20) >> 5);
     Serial.println((buffer[1] & 0x40) >> 6);
     Serial.println((buffer[1] & 0x80) >> 7);*/

    int o=0;
    for(int m = 7; m >=0; m--) {
      int pos = 1;
      for(int n = m; n > 0 ; n--) {
        pos *= 2;
      }
      relays[o] = (( (buffer[0] & pos) >> m) == 0x01 ? true : false);
      o++;
    }    
    for(int m = 15; m >=8; m--) {
      int pos = 1;
      for(int n = (m-8); n > 0 ; n--) {
        pos *= 2;
      }
      relays[o] = (( (buffer[1] & pos) >> (m-8)) == 0x01 ? true : false);
      o++;
    }

  }// end if(strbuf == "ask//")
}


boolean toFlip = false;
void printStatustoLCD() {
  if(!toFlip) {
    toFlip = true;
    lcd.clear();
    //Serial.println("Relay Statuses:\r\n---------");
    for(int o = 0; o<16; o++) {
      //Serial.print(o+1);
      //Serial.print(": ");
      if(relays[o]) {
        lcd.write(relayONChar);
      } 
      else {
        lcd.write(relayOFFChar);
      }
      //lcd.print(relays[o]);
    }
    lcd.setCursor(0,2);
    //Serial.println("=========");
    //Serial.println("States:\r\n---------");
    //Serial.print("curr: ");
    lcd.print(curr_thermostat_state);
    //Serial.print("last: ");
    lcd.print(last_thermostat_state);
    //Serial.print("user: ");
    lcd.print(user_thermostat_state);
    lcd.print(" ");
    //Serial.print("tooHot: ");
    lcd.print(tooHot);
    //Serial.print("tooCold: ");
    lcd.print(tooCold);
    lcd.print(" ");  
    //Serial.print("Desired Temp: ");
    lcd.print(DESIRED_TEMP + degreeCorrectionF);
    lcd.write(degreeChar);
    lcd.print(" ");  
    //Serial.print("userChanged: ");
    lcd.print(userChanged);  
    //Serial.print("haveRestored: ");
    lcd.print(haveRestored);
  } 
  else {
    toFlip = false;
    printTemptoLCD();
  }
}

void printTemptoLCD() {
  lcd.clear();
  lcd.print("Temperature:");
  lcd.setCursor(0,2);
  lcd.print(tempF);
  lcd.write(degreeChar);
  lcd.print(" F ");

  // if we're set to COOL and !tooHot or set to HEAT and !tooCold and justRight
  if((!tooHot && user_thermostat_state == COOL) || (!tooCold && user_thermostat_state == HEAT) || (justRight && user_thermostat_state == OFF) || (justRight && user_thermostat_state == FAN)) {
    lcd.write(smileyChar);
  }
  else {
    lcd.write(frownieChar);
  }
}

void printUptimetoLCD(char* intbuffer) {
  // write to LCD
  lcd.clear();
  lcd.print("Uptime:");
  lcd.setCursor(0,2);
  lcd.print(intbuffer);
  lcd.print(" seconds");
}

/*
 * getVoltage() - returns the voltage on the analog input defined by
 * pin
 */
float getVoltage(int pin){
  return (analogRead(pin) * aref_voltage_33 / 1024.0); //converting from a 0 to 1023 digital range
  // to 0 to 5 volts (each 1 reading equals ~ 5 millivolts
}

















