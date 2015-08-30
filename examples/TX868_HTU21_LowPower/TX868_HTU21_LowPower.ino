/*
 * Production code to build a weather sensor/transmitter 
 * similar to the S300 by ELV.
 *
 * It measures temperature and humidity with a HTU21D sensor
 * and transmits these values using the TX868.
 * The address of the transmitter may be configured with a DIP switch.
 * The processor is put into power down sleep and waked up 
 * by the watchdog timer periodically to reduce power consumption.
 *
 * Required libraries:
 *   - TempHygroTX868: https://github.com/skaringa/TempHygroTX868
 *   - SparkFunHTU21D: https://github.com/sparkfun/SparkFun_HTU21D_Breakout_Arduino_Library
 *   
 * Hardware setup: 
 *   - TX868 Data pin connected to digital pin 5
 *   - HTU21D SDA connected to A4 and SCL to A5
 *   - 3 DIP switches are connected to digital pins 7, 8, 9 with one contact
 *     and to ground with the other contact
 */


#include <TempHygroTX868.h>
#include <Wire.h>
#include <SparkFunHTU21D.h>


#include <avr/sleep.h>
#include <avr/power.h>


HTU21D htu;
TempHygroTX868 tx;

// transmission timer:
// it holds the number of seconds remaining until the next transmission
volatile int nextTxTimer;

// pin of build-in signal LED
#define LED 13

// lowest pin of DIP switch
#define SWITCH 7

// you may define DEBUG macro here
// to get debug output at the serial line


void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println();
  Serial.println("Address\tHumidity (%)\tTemperature (C)");
#endif

  htu.begin();
  tx.setup(5); // transmitter is at data pin 5
  pinMode(LED, OUTPUT); // signal LED
  
  nextTxTimer = 0;
  
  /* Setup the Watchdog timer */
  // clear the reset flag
  MCUSR &= ~(1<<WDRF);
  // enable configuration changes
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout prescaler value to maximum
  WDTCSR = 1<<WDP0 | 1<<WDP3; 
  // Enable the watchdog interrupt
  WDTCSR |= _BV(WDIE);
}

void loop()
{
  // are we required to send?
  if (nextTxTimer <= 9) {
    // yes - delay remaining time
    delay(nextTxTimer * 1000);
    // reset time to next transmission
#ifdef DEBUG
    nextTxTimer = 30;    
#else
    nextTxTimer = tx.getPause();
#endif

    // read address from DIP switch
    byte addr = readSwitch();
#ifdef DEBUG
    Serial.print(addr);
    Serial.print("\t");
#endif
    
    tx.setAddress(addr);
    // read sensor and transmit
    sendData();
  } 
  // go into power down sleep
  pwrDownSleep();
}

/*
 * Read address from DIP switch
 */
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

/*
 * Read data from sensor and transmit.
 */
void sendData() 
{
  digitalWrite(LED, HIGH);
  float humidity = htu.readHumidity();
  float temperature = htu.readTemperature();
 
#ifdef DEBUG
  Serial.print(humidity);
  Serial.print("\t");
  Serial.println(temperature);
#endif
  
  delay(100);
  
  if (humidity < 900 && temperature < 900) {
    // valid reading
    tx.send(temperature, humidity);
  }
  digitalWrite(LED, LOW);
}

/*
 * Interrupt service routine triggered by watchdog.
 */
ISR(WDT_vect)
{
  // Watchdog oscillator freq is about 116 kHz
  // at 3 V and 25 °C
  // Therefore time between interrupts is
  // 1,048,576 / 116,000 = 9.039 seconds.
  // So decrement the transmission timer by this value.
  nextTxTimer -= 9;
}

/*
 * Go into power down mode
 */
void pwrDownSleep()
{
  byte adcsra = ADCSRA;
  ADCSRA = 0;  // disable the ADC - saves 80 µA
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  cli(); // stop interrupts
  sleep_bod_disable(); // disable brown-out detection
  sei(); // enable interrupts
  sleep_cpu();
  // At this point the CPU is sleeping until watchdog bites
  sleep_disable();
  power_all_enable();
  ADCSRA = adcsra; // restore ADCSRA
}
