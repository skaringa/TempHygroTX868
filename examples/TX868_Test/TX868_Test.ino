/*
 * Sample program to test the TX868.
 *
 * It sends fixed values for address, temperature, and humidity.
 * 
 * Hardware setup: 
 *   TX868 Data pin connected to digital pin 5
 */

#include <TempHygroTX868.h>

TempHygroTX868 tx;

void setup()
{
  tx.setup(5); // transmitter is at data pin 5
}

void loop()
{
  byte address = 3;
  float humidity = 50;
  float temperature = 21.4;

  tx.setAddress(address);
  tx.send(temperature, humidity);

  delay(10000);
}
