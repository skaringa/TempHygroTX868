/*
 * Production code to simulate a S300 sensor.
 *
 * It records temperature and humidity with a DHT22 sensor
 * and send these values with the TX868.
 * The address may be configured with a DIP switch.
 * The processor is put into power down sleep and waked up
 * by the watchdog periodically to reduce the power consumption.
 *
 * Software setup:
 *   DHT library from markruys is required: 
 *   https://github.com/markruys/arduino-DHT.git
 *   
 * Hardware setup: 
 *   - TX868 Data pin connected to digital pin 5
 *   - DHT11 Data pin connected to digital pin 6
 *   - 3 DIP switches connected between digital pins 7, 8, 9 
 *     at one side and ground at the other side
 */


#include <TempHygroTX868.h>
#include <DHT.h>


#include <avr/sleep.h>
#include <avr/power.h>


DHT dht;
TempHygroTX868 tx;
volatile int nextTxTimer;

#define LED 13

#define SWITCH 7

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println();
  Serial.println("Address\tStatus\tHumidity (%)\tTemperature (C)");
#endif

  dht.setup(6, DHT::DHT22); // sensor is at data pin 6
  tx.setup(5); // transmitter is at data pin 5
  pinMode(LED, OUTPUT); // signal LED
  
  nextTxTimer = 0;
  
  /* Setup the Watchdog timer */
  // clear the reset flag.
  MCUSR &= ~(1<<WDRF);
  
  // enable configuration changes
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // set new watchdog timeout prescaler value
  WDTCSR = 1<<WDP0 | 1<<WDP3; // 8.0 seconds
  
  // Enable the WD interrupt
  WDTCSR |= _BV(WDIE);
}

void loop()
{
  if (nextTxTimer <= 9) {
    delay(nextTxTimer * 1000);
#ifdef DEBUG
    nextTxTimer = 30;    
#else
    nextTxTimer = tx.getPause();
#endif

    byte addr = readSwitch();
#ifdef DEBUG
    Serial.print(addr);
    Serial.print("\t");
#endif
    
    tx.setAddress(addr);
    sendData();
    pwrDownSleep();
  } else {
    pwrDownSleep();
  }
}

byte readSwitch()
{
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(SWITCH+1, INPUT_PULLUP);
  pinMode(SWITCH+2, INPUT_PULLUP);
  delay(1);
  
  byte val = digitalRead(SWITCH+2);
  val <<= 1;
  val |= digitalRead(SWITCH+1);
  val <<= 1;
  val |= digitalRead(SWITCH);
  
  // disconnect pullups to save power
  pinMode(SWITCH, INPUT);
  pinMode(SWITCH+1, INPUT);
  pinMode(SWITCH+2, INPUT);
  
  return val;
}

void sendData() 
{
  digitalWrite(LED, HIGH);
  for (int i = 0; i < 5; ++i) {
    // try 5 times to get a valid reading from DHT
    delay(dht.getMinimumSamplingPeriod());
  
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
 
#ifdef DEBUG
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(humidity);
    Serial.print("\t");
    Serial.println(temperature);
#endif
    
    delay(100);
    
    if (DHT::ERROR_NONE == dht.getStatus()) {
      tx.send(temperature, humidity);
      break; // valid reading - exit loop
    }
  }
  digitalWrite(LED, LOW);
}

// interrupt service routine triggered by watchdog
ISR(WDT_vect)
{
  // Watchdog oscillator freq is about 116 kHz
  // at 3 V and 25 °C
  // Therefore time between interrupts is
  // 1,048,576 / 116,000 = 9.039 seconds
  nextTxTimer -= 9;
}

// go into power down mode
void pwrDownSleep()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  // Sleeping until watchdog bites
  sleep_disable();
  power_all_enable();
}
