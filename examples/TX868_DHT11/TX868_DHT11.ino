/*
 * Sample program to test the TX868.
 *
 * It sends temperature and humidity values 
 * that were measured with a DHT11 or DHT22 sensor.
 * The address is hardcoded.
 * 
 * Software setup:
 *   DHT library from markruys is required: 
 *   https://github.com/markruys/arduino-DHT.git
 *   
 * Hardware setup: 
 *   - TX868 Data pin connected to digital pin 5
 *   - DHT11 Data pin connected to digital pin 6
 */
 
#include <TempHygroTX868.h>
#include <DHT.h>

DHT dht;
TempHygroTX868 tx;

void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println("Address\tStatus\tHumidity (%)\tTemperature (C)");

  dht.setup(6, DHT::DHT11); // sensor is at data pin 6
  tx.setup(5); // transmitter is at data pin 5
}

void loop()
{
  byte addr = 3;
  Serial.print(addr);
  Serial.print("\t");
  
  tx.setAddress(addr);
  sendData();

  delay((unsigned long)tx.getPause() * 1000UL);
}

void sendData() 
{
  for (int i = 0; i < 5; ++i) {
    // try 5 times to get a valid reading from DHT
    delay(dht.getMinimumSamplingPeriod());
  
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
 
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(humidity);
    Serial.print("\t");
    Serial.println(temperature);
    
    delay(100);
    
    if (DHT::ERROR_NONE == dht.getStatus()) {
      tx.send(temperature, humidity);
      break; // valid reading - exit loop
    }
  }
}
