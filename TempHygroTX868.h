#ifndef _TempHygroTX868_h
#define _TempHygroTX868_h

#if ARDUINO < 100
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif

class TempHygroTX868 {
public:
  void setup(byte pin);
  void setAddress(byte addr);
  void send(float temp, float humidity = 0);
  int getPause();
  
private:
  void sendData(byte* data, byte length);
  void sendBit(byte value);
  void sendNibble(byte value);

  byte pin;
  byte addr;
  unsigned long nextSlope;
};

#endif

