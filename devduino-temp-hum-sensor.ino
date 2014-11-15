/*
 * devduino-temp-hum-sensor.ino - Firmware for DevDuino v2.0 based temperature and humidity sensor Node with nRF24L01+ module
 *
 * Copyright 2014 Tomas Hozza <thozza@gmail.com>
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
 * DevDuino v2.0 - http://www.seeedstudio.com/wiki/DevDuino_Sensor_Node_V2.0_(ATmega_328)
 * nRF24L01+ spec - https://www.sparkfun.com/datasheets/Wireless/Nordic/nRF24L01P_Product_Specification_1_0.pdf
 */

#include <MyMessage.h>
#include <MySensor.h>

#include <SPI.h>
#include <DHT.h>
#include <stdint.h>

#define DEBUG 1


/***********************************/
/********* PIN DEFINITIONS *********/
/***********************************/
#define LED_pin 9
#define DHT22_pin 3
#define RF24_CE_pin 8
#define RF24_CS_pin 7
#define MCP9700_pin A3

/*****************************/
/********* FUNCTIONS *********/
/*****************************/
float readMCP9700Temp();
float readDHT22Hum();
float readDHT22Temp();
uint16_t readVcc();
/************************************/
/********* GLOBAL VARIABLES *********/
/************************************/
DHT dht22;

/**********************************/
/********* IMPLEMENTATION *********/
/**********************************/
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
}

/**
 * Read the humidity from DHT22
 */
float readDHT22Hum() {
  float hum = dht22.getHumidity();
  #if DEBUG
  Serial.print("Read Hum from DHT22 = ");
  Serial.println(hum);
  #endif
  return hum;
}

/**
 * Read the temperature from DHT22
 */
float readDHT22Temp() {
  float temp = dht22.getTemperature();
  #if DEBUG
  Serial.print("Read Temp from DHT22 = ");
  Serial.println(temp);
  #endif
  return temp;
}

/**
 * Read the temperature from MCP9700
 */
float readMCP9700Temp() {
  float temp = analogRead(MCP9700_pin)*3.3/1024.0;
  temp = temp - 0.5;
  temp = temp / 0.01;
  #if DEBUG
  Serial.print("Read Temp from MCP9700 = ");
  Serial.println(temp);
  #endif
  return temp;
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
  
  #if DEBUG
  Serial.print("Read Vcc = ");
  Serial.println(result);
  #endif
  
  return (uint16_t)result; // Vcc in millivolts
}
