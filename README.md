# Temperature and Humidity sensor based on Devduino v2.0

This is a simple sketch for temperature and humidity wireless sensor
based on [MySensors library](http://www.mysensors.org/). For more info
about the requirements, please read on.

## Hardware Prerequisites

The shetch is intended for Arduino compatible board [Devduino v2.0](http://www.seeedstudio.com/wiki/DevDuino_Sensor_Node_V2.0_(ATmega_328)). The board comes with MCP9700 chip, which is an analog temperature sensor. This sketch also uses digital temperature and humidity sensor [DHT22](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf) and/or sensor [HTU21/SHU21](https://www.adafruit.com/product/1899)for maximum precision. The sensor node is wireless, and uses the [nRF24L01+ wireless module](https://www.sparkfun.com/datasheets/Wireless/Nordic/nRF24L01P_Product_Specification_1_0.pdf) for communication with sensor gateway.

### Complete List

* [Devduino v2.0 board](http://www.seeedstudio.com/wiki/DevDuino_Sensor_Node_V2.0_(ATmega_328))
* [nRF24L01+ wireless module](https://www.sparkfun.com/datasheets/Wireless/Nordic/nRF24L01P_Product_Specification_1_0.pdf)
* [DHT22 sensor](http://www.seeedstudio.com/depot/Grove-TemperatureHumidity-Sensor-Pro-p-838.html) [(datasheet)](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf)
* [HTU21/SHU21 sensor](https://www.adafruit.com/product/1899)

## Software Prerequisites a.k.a. Libraries

Library dependencies are handled by PlatformIO. For the complete list, please see **platformio.ini** file inside the repository.

## How it works?

The sketch does not have the **node ID** set. Its assignment is left up to MySensors library. If you want to hardcode **node ID**, uncomment and define the ```MY_NODE_ID``` in the sketch. The shetch also uses the default settings of [MySensors library](http://www.mysensors.org/) for the **nRF24L01+** module (like channel, etc.). The sensor node wakes up from sleep every **5 minutes** after sending all values to the gateway. To change this, redefine the ```SLEEP_TIME``` to different value in **milliseconds**.

### How to choose connected sensors

To choose connected sensors, just comment out one of the following lines in the sketch:

```
// Comment out if you don't want to use these sensors
#define WITH_SENSOR_DHT22     // connected to the DIGITAL connector
#define WITH_SENSOR_MCP9700   // on board
#define WITH_SENSOR_HTU21     // connected to the I2C connector
```

### Sensor Setup and Presentation Phase
1. Initialization of the sensor node
2. Send the sketch info to the gateway
3. Present all connected sensors
  * Sensor ID **0** - DHT22 sensor humidity
  * Sensor ID **1** - DHT22 sensor temperature
  * Sensor ID **2** - MCP9700 sensor temperature
  * Sensor ID **3** - HTU21 sensor humidity
  * Sensor ID **4** - HTU21 sensor temperature

### Sensor Execution Phase
* Each **5 minutes** do the following:

1. send battery level in percents
2. send DHT22 sensor humidity
3. send DHT22 sensor temperature
4. send MCP9700 sensor temperature
5. send HTU21 sensor humidity
6. send HTU21 sensor temperature
7. go to sleep
