/*
 * Sample program to test the TX868.
 *
 * It transmits temperature and humidity values 
 * that were measured with a DHT11 or DHT22 sensor.
 * The address of the transmitter is hardcoded.
 * 
 * Required libraries:
 *   - TempHygroTX868: https://github.com/skaringa/TempHygroTX868
 *   - DHT: https://github.com/markruys/arduino-DHT
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
  // serial used only for debugging purposes
  Serial.begin(9600);
  Serial.println();
  Serial.println("Address\tStatus\tHumidity (%)\tTemperature (C)");

  // setup sensor and transmitter
  dht.setup(6, DHT::DHT11); // sensor is at data pin 6
  tx.setup(5); // transmitter is at data pin 5
}

void loop()
{
  byte addr = 3; // address of transmitter (0..7)
  Serial.print(addr);
  Serial.print("\t");
  
  tx.setAddress(addr);
  // read sensor and transmit
  sendData();

  delay((unsigned long)tx.getPause() * 1000UL);
}

/*
 * Read data from sensor and transmit.
 */
void sendData() 
{
  for (int i = 0; i < 5; ++i) {
    // try 5 times to get a valid reading from DHT
    delay(dht.getMinimumSamplingPeriod());
  
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
 
    // debug output of values to serial
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(humidity);
    Serial.print("\t");
    Serial.println(temperature);
    
    delay(100);
    
    if (DHT::ERROR_NONE == dht.getStatus()) {
      // valid reading: send values with transmitter
      tx.send(temperature, humidity);
      break; // exit loop
    }
  }
}
