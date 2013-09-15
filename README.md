Arduino-Thermostat
==================

Arduino UNO with a USB Host Shield, a 16x2 LCD, and a TMP36 Temperature Sensor from SparkFun plugged into a PIC/relay board from Denkovi controlling relays over FTDI USB->Serial link

TMP36 Sensor: https://www.sparkfun.com/products/10988

TMP36 uses Analog Pin to read in a voltage from the sensor.

16x2 LCD: https://www.sparkfun.com/products/709

LCD is using LiquidCrystal library already included in Arduino IDE: http://arduino.cc/en/Tutorial/LiquidCrystal

USB Host Shield: https://www.sparkfun.com/products/9947

USB Host Shield Library from here: https://github.com/felis/USB_Host_Shield_2.0.git

Note: Sparkfun USB Host Shield differs from Circuits@Home kind. Pin7 on Arduino UNO must be brought HIGH in order to bring max3421e online.

The PIC board being used is this one: http://denkovi.com/productv/33/usb-16-channel-relay-module-rs232-controlled-12v-ver-2.html

Android Arduino-ThermostatController
====================================

This is the companion Android application to use on Android devices that support USBHost mode to talk with Arduino over its USB->Serial CDC_ACM interface.

It controls the Arduino-Thermostat without the need for a PC. Testing on Galaxy Nexus and Nexus 10 so far.

It is accomplished with the help from this open source USB->Serial driver/library for the USBHost API's in Android: https://github.com/mik3y/usb-serial-for-android.git
