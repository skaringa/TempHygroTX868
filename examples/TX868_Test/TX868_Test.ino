/*
 * Sample program to test the HFS-300.
 *
 * It transmits fixed values for address, temperature, and humidity.
 *
 * Required libraries:
 *   TempHygroTX868: https://github.com/skaringa/TempHygroTX868
 * 
 * Hardware setup: 
 *   TX868 Data pin connected to digital pin 5
 */

#include <TempHygroTX868.h>

TempHygroTX868 tx;

void setup()
{
  // TX868 is at data pin 5
  // Use the old protocol V1.1
  tx.setup(5, TempHygroTX868::PROT_V11);
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
