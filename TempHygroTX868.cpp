#include "TempHygroTX868.h"

void TempHygroTX868::setup(byte pin) {
  this->pin = pin;
  this->addr = 0;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  nextSlope = 0;
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
  sendData(data, 8);
}

int TempHygroTX868::getPause() {
  return 177 - addr;
}

void TempHygroTX868::sendBit(byte value) {
  if (nextSlope > micros()) {
    delayMicroseconds(nextSlope - micros());
  }
  
  digitalWrite(pin, HIGH);
  nextSlope = micros() + 1220;
  
  if (value) {
    delayMicroseconds(366);
  } else {
    delayMicroseconds(854);
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
  for (byte i = 0; i < 10; ++i) {
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
  sendNibble(sum + check + 5);
  
  // ensure that tx is off
  digitalWrite(pin, LOW);
}
