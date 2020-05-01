/*
 * Arduino library to control the ELV TX868 rf transmitter module
 * to send temperature and humidity values over the air at 868.35 MHz.
 * The communication protocol is compatible to ELV sensors like the
 * S 300 and ASH 2200, therefore the data may be received by weather
 * stations like USB-WDE 1, WS 200/300, and IPWE 1 manufactured by ELV 
 * (http://www.elv.de).
 * The library has been extended to implement the old ELV sensor
 * transmission protocol V1.1 as well. This should support 
 * Thermo/Hygro sensors like the AS2000 and ASH2000 which are using the
 * 433 MHz transmitter HFS-300.
 * 
 * Copyright 2015, 2020 Martin Kompf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TempHygroTX868.h"

void TempHygroTX868::setup(byte pin, byte protocol) {
  this->pin = pin;
  this->addr = 0;
  this->protocol = protocol;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  nextSlope = 0;
  if (protocol == PROT_V11) {
    this->t1 = 610;
    this->t0 = 1221;
    this->prefixLen = 16;
    this->repeatCount = 3;
  } 
  else { // PROT_V12
    this->t1 = 366;
    this->t0 = 854;
    this->prefixLen = 10;
    this->repeatCount = 1;
  }
}

void TempHygroTX868::setAddress(byte addr) {
  this->addr = addr;
}

void TempHygroTX868::send(float temp, float humidity) {
  // prepare data in memory
  int ihum = int(humidity*10.0);
  int itemp = int(abs(temp*10.0));
  
  byte data[8]; 
  data[0] = 1; // type 1: thermo/hygro (S300 etc)
  data[1] = (addr & 0x7) | (temp < 0.0? 0x8 : 0); // address | sign
  for (byte i = 2; i < 5; ++i) { 
    data[i] = itemp % 10;
    itemp /= 10;
  }
  for (byte i = 5; i < 8; ++i) { 
    data[i] = ihum % 10;
    ihum /= 10;
  }
  
  // send data with transmitter
  int count = repeatCount;
  do {
    sendData(data, 8);
    if (--count == 0) {
      break;
    }
    delay(100);
  } while(1);
}

int TempHygroTX868::getPause() {
  return 177 - addr;
}

void TempHygroTX868::sendBit(byte value) {
  if (nextSlope > micros()) {
    delayMicroseconds(nextSlope - micros());
  }
  
  digitalWrite(pin, HIGH);
  nextSlope = micros() + t1 + t0;
  
  if (value) {
    delayMicroseconds(t1);
  } else {
    delayMicroseconds(t0);
  }
  digitalWrite(pin, LOW);
}

void TempHygroTX868::sendNibble(byte value) {
  for (byte i = 0; i < 4; ++i) {
    sendBit(value & 1);
    value >>= 1;
  }
  sendBit(1); // end of nibble
}

void TempHygroTX868::sendData(byte* data, byte length) {
  int sum = 0;
  byte check = 0;
  byte* nibble = &data[0];
  
  // sync
  for (byte i = 0; i < prefixLen; ++i) {
    sendBit(0);
  }
  // start bit
  sendBit(1);
  // data  
  for (byte i = 0; i < length; ++i) {
    check ^= *nibble;
    sum += *nibble;
    sendNibble(*nibble);
    ++nibble;
  }
  
  // check sum
  sendNibble(check);
  if (protocol == PROT_V12) {
    sendNibble(sum + check + 5);
  }
  
  // ensure that tx is off
  digitalWrite(pin, LOW);
}
