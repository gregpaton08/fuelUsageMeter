/*! \file fuelUsageMeter.ino
 *  \brief Arduino sketch for monitoring instantaneous
 *         fuel usage usind OBD and OLED display.
 *  \author Greg Paton
 *  \date 16 May 2013
 */
// Copyright (c) GSP 2013


#include "OBDLib.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_DC 11
#define OLED_CS 12
#define OLED_CLK 10
#define OLED_MOSI 9
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

OBDLib obd;


void setup() {
  // set BAUD rate
  Serial.begin(38400);
  
  // Setup OBD
  obd.init(); 
  
  // Setup display 
  display.begin();
  display.clearDisplay();
  for (uint8_t i = 0; i < 64; ++i) {
    display.drawLine(0, 63 - i, 127, 63 - i, 1);
    display.display();
  }
}


void loop() {
  uint8_t maf = getMAF();
  if (maf != 0) {
    display.clearDisplay();
    setHorizontalGauge((maf * 90) / 100);
    display.display();
  }
}

uint8_t getMAF() {
  byte pid = 0x10;
  uint8_t len = 0;
  uint8_t pidResSize = 10;
  char pidRes[pidResSize];
  
  // Query PID
  obd.sendCMD(0x01, pid);
  
  uint8_t strPidSize = 4;
  char strPid[strPidSize];
  String strPidtemp = String(pid, HEX);
  strPidtemp.toUpperCase();
  if (strPidtemp.length() == 1) {
      strPidtemp = "0" + strPidtemp;
  }
  strPidtemp.toCharArray(strPid, strPidSize);
  
  while (true) {
    if (Serial.find(strPid))
      break;
    if (Serial.find("UNABLE TO CONNECT")) {
      return 0;
    }
  }
   
  // loop until new line character found
  while (len < pidResSize) {
    unsigned char c = Serial.read();
    if (c == (unsigned char)-1 || c == 32) continue;
    if (c == '\n' || c == '\r') break;
    pidRes[len] = c;
    ++len;
  }
  
  String ret = String(pidRes);
  
  return ret.toInt();
}

void setVerticalGauge(int8_t percent) {
  if (percent < 0)
    percent = 0;
  else if (percent > 100)
    percent = 100;
    
  int8_t height = ((display.height() - 1) * percent) / 100;
    
  display.fillRect(0, display.height() - height, display.width(), height, 1);
  display.display();
}

void setHorizontalGauge(int8_t percent) {
  if (percent < 0)
    percent = 0;
  else if (percent > 100)
    percent = 100;
    
  int8_t width = ((display.width() - 1) * percent) / 100;
    
  display.fillRect(0, 0, width, display.height(), 1);
  display.display();
}
