/*
 * devduino-temp-hum-sensor.ino - Firmware for DevDuino v2.0 based temperature and humidity sensor Node with nRF24L01+ module
 *
 * Copyright 2014-2018 Tomas Hozza <thozza@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Tomas Hozza <thozza@gmail.com>
 *
 * MySensors library - http://www.mysensors.org/
 * DHT library - https://github.com/adafruit/DHT-sensor-library
 * SHT21/HTU21 library - https://github.com/adafruit/Adafruit_HTU21DF_Library
 * DevDuino v2.0 - http://www.seeedstudio.com/wiki/DevDuino_Sensor_Node_V2.0_(ATmega_328)
 *               - https://seeeddoc.github.io/DevDuino_Sensor_Node_V2.2-ATmega_328/
 * nRF24L01+ spec - https://www.sparkfun.com/datasheets/Wireless/Nordic/nRF24L01P_Product_Specification_1_0.pdf
 *
 *  * Version:
 * 0.2
 *  - port to MySensors 2.3.0
 *  - added HTU21 sensor support
 *  - refactored code to allow selection of connected sensors
 *
 */

#define MY_DEBUG
#define MY_DEBUG_VERBOSE
//#define MY_DEBUG_VERBOSE_RF24
//#define MY_SIGNAL_REPORT_ENABLED

#define MY_RADIO_RF24

#define MY_RF24_CE_PIN 8
#define MY_RF24_CS_PIN 7
#define MY_RF24_IRQ_PIN 2

/**
 * - 0 => 2400 Mhz (RF24 channel 1)
 * - 1 => 2401 Mhz (RF24 channel 2)
 * - 76 => 2476 Mhz (RF24 channel 77)
 * - 83 => 2483 Mhz (RF24 channel 84)
 * - 124 => 2524 Mhz (RF24 channel 125)
 * - 125 => 2525 Mhz (RF24 channel 126)
 * @see http://www.bundesnetzagentur.de/SharedDocs/Downloads/DE/Sachgebiete/Telekommunikation/Unternehmen_Institutionen/Frequenzen/Allgemeinzuteilungen/2013_10_WLAN_2,4GHz_pdf.pdf
 */
#define MY_RF24_CHANNEL         76             //RF channel for the sensor net, 0-127
#define MY_RF24_DATARATE        RF24_250KBPS   //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
#define MY_RF24_PA_LEVEL 	      RF24_PA_MAX    //Sensor PA Level == RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, and RF24_PA_MAX=0dBm
#define MY_RF24_BASE_RADIO_ID   0x00,0xFC,0xE1,0xA8,0xA8

//#define MY_RX_MESSAGE_BUFFER_FEATURE

//#define MY_NODE_ID 1
//#define MY_PARENT_NODE_ID 0

#define MY_DEFAULT_LED_BLINK_PERIOD 300

#define MY_DEFAULT_ERR_LED_PIN 9  // Error led pin - Built in LED on board
#define MY_DEFAULT_RX_LED_PIN  A6  // Receive led pin - Not connected / used
#define MY_DEFAULT_TX_LED_PIN  A7  // Transmit led pin - Not connected / used

#define MY_SPLASH_SCREEN_DISABLED

#include <MySensors.h>

/**************************** SKETCH DEFINITIONS ****************************/

// Comment out if you don't want to use these sensors
#define WITH_SENSOR_DHT22     // connected to the DIGITAL connector
#define WITH_SENSOR_MCP9700   // on board
#define WITH_SENSOR_HTU21     // connected to the I2C connector

#ifdef WITH_SENSOR_DHT22
#include <DHT.h>
#endif

#ifdef WITH_SENSOR_HTU21
#include <Wire.h>
#include <Adafruit_HTU21DF.h>
#endif

#include <stdint.h>

#define SLEEP_TIME 300000  // 5min

#ifdef WITH_SENSOR_DHT22
#define SENSOR_DHT22_HUM 0
#define SENSOR_DHT22_TEMP 1

#define DHT_PIN 3
#define DHT_TYPE DHT22

float readDHT22Hum();
float readDHT22Temp();

MyMessage msgDHT22Hum(SENSOR_DHT22_HUM, V_HUM);
MyMessage msgDHT22Temp(SENSOR_DHT22_TEMP, V_TEMP);

DHT dht(DHT_PIN, DHT_TYPE);
#endif /* WITH_SENSOR_DHT22 */

#ifdef WITH_SENSOR_MCP9700
#define SENSOR_MCP9700_TEMP 2

#define MCP9700_pin A3

float readMCP9700Temp();

MyMessage msgMCP9700Temp(SENSOR_MCP9700_TEMP, V_TEMP);
#endif /* WITH_SENSOR_MCP9700 */

#ifdef WITH_SENSOR_HTU21
#define SENSOR_HTU21_HUM 3
#define SENSOR_HTU21_TEMP 4

float readHTU21Hum();
float readHTU21Temp();

MyMessage msgHTU21Hum(SENSOR_HTU21_HUM, V_HUM);
MyMessage msgHTU21Temp(SENSOR_HTU21_TEMP, V_TEMP);

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
#endif /* WITH_SENSOR_HTU21 */

uint16_t readVcc();
uint8_t getVccLevel();

const char *sketch_name = "devduino-temp-hum-rf24";
const char *sketch_version = "0.2";

/**********************************/
/********* IMPLEMENTATION *********/
/**********************************/
void setup()
{
  #ifdef WITH_SENSOR_DHT22
  dht.begin();
  #endif
  #ifdef WITH_SENSOR_HTU21
  htu.begin();
  #endif
}

void presentation()
{
  sendSketchInfo(sketch_name, sketch_version, false);

  #ifdef WITH_SENSOR_DHT22
  present(SENSOR_DHT22_HUM, S_HUM, "DHT22 Humidity");
  present(SENSOR_DHT22_TEMP, S_TEMP, "DHT22 Temperature");
  #endif
  #ifdef WITH_SENSOR_MCP9700
  present(SENSOR_MCP9700_TEMP, S_TEMP, "MCP9700 Temperature");
  #endif
  #ifdef WITH_SENSOR_HTU21
  present(SENSOR_HTU21_HUM, S_HUM, "HTU21 Humidity");
  present(SENSOR_HTU21_TEMP, S_TEMP, "HTU21 Temperature");
  #endif
}

void loop() {
  sendBatteryLevel(getVccLevel());

  #ifdef WITH_SENSOR_DHT22
  send(msgDHT22Hum.set(readDhtHum(), 2));
  send(msgDHT22Temp.set(readDhtTemp(), 2));
  #endif
  #ifdef WITH_SENSOR_MCP9700
  send(msgMCP9700Temp.set(readMCP9700Temp(), 2));
  #endif
  #ifdef WITH_SENSOR_HTU21
  send(msgHTU21Hum.set(readHTU21Hum(), 2));
  send(msgHTU21Temp.set(readHTU21Temp(), 2));
  #endif

  smartSleep(SLEEP_TIME);
}

#ifdef WITH_SENSOR_DHT22
/**
 * Read the humidity from DHTXX
 */
float readDhtHum() {
  float hum = dht.readHumidity();
  #ifdef MY_DEBUG
  Serial.print(F("Read Hum from DHT22 = "));
  Serial.println(hum);
  #endif
  return hum;
}

/**
 * Read the temperature from DHTXX
 */
float readDhtTemp() {
  float temp = dht.readTemperature();
  #ifdef MY_DEBUG
  Serial.print(F("Read Temp from DHT22 = "));
  Serial.println(temp);
  #endif
  return temp;
}
#endif /* WITH_SENSOR_DHT22 */

#ifdef WITH_SENSOR_MCP9700
/**
 * Read the temperature from MCP9700
 */
float readMCP9700Temp() {
  float temp = analogRead(MCP9700_pin)*3.3/1024.0;
  temp = temp - 0.5;
  temp = temp / 0.01;
  #ifdef MY_DEBUG
  Serial.print(F("Read Temp from MCP9700 = "));
  Serial.println(temp);
  #endif
  return temp;
}
#endif /* WITH_SENSOR_MCP9700 */

#ifdef WITH_SENSOR_HTU21
/**
 * Read the humidity from HTU21
 */
float readHTU21Hum() {
  float hum = htu.readHumidity();
  #ifdef MY_DEBUG
  Serial.print(F("Read Hum from HTU21 = "));
  Serial.println(hum);
  #endif
  return hum;
}

/**
 * Read the temperature from HTU21
 */
float readHTU21Temp() {
  float temp = htu.readTemperature();
  #ifdef MY_DEBUG
  Serial.print(F("Read Temp from HTU21 = "));
  Serial.println(temp);
  #endif
  return temp;
}
#endif /* WITH_SENSOR_HTU21 */

/**
 * Get the percentage of power in the battery
 */
uint8_t getVccLevel() {
  long vcc = readVcc();
  vcc -= 1900;    // 1.9 V will be the lowest operational voltage
  uint8_t percent = vcc / 14.0;
  #ifdef MY_DEBUG
  Serial.print(F("Battery state = "));
  Serial.println(percent);
  #endif
  return percent;
}

/**
 * Measure remaining voltage in battery in millivolts
 *
 * From http://www.seeedstudio.com/wiki/DevDuino_Sensor_Node_V2.0_(ATmega_328)#Measurement_voltage_power
 */
uint16_t readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(75); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  
  #ifdef MY_DEBUG
  Serial.print(F("Read Vcc = "));
  Serial.println(result);
  #endif
  
  return (uint16_t)result; // Vcc in millivolts
}
