/*
 * Arduino library to control the ELV TX868 rf transmitter module
 * to send temperature and humidity values over the air at 868.35 MHz.
 * The communication protocol is compatible to ELV sensors like the
 * S 300 and ASH 2200, therefore the data may be received by weather
 * stations like USB-WDE 1, WS 200/300, and IPWE 1 manufactured by ELV 
 * (http://www.elv.de).
 * 
 * Copyright 2015 Martin Kompf
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
 
#ifndef _TempHygroTX868_h
#define _TempHygroTX868_h

#if ARDUINO < 100
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif

/*
 * Controller for the TX868 rf transmitter. 
 */
class TempHygroTX868 {
public:
  /*
   * Setup the controller.
   * This should be called once during setup() routine.
   * Parameter pin: The digitial pin number at which the TX868 is connected.
   */
  void setup(byte pin);

  /*
   * Sets the address of the transmitter.
   * May be called multiple times.
   * The address will be part of the data to send and is used to
   * distinguish different transmitters.
   * Parameter addr: The address (0..7).
   */
  void setAddress(byte addr);

  /*
   * Sends the data.
   * Parameter temp: Temperature in °C 
   * Parameter humidity: Relative humidity in %
   */
  void send(float temp, float humidity = 0);

  /*
   * Computes the length of the pause between transmissions.
   * Returns: The pause in seconds.
   */
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

