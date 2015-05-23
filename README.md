# Temperature and Humidity sensor based on Devduino v2.0

This is a simple sketch for temperature and humidity wireless sensor
based on [MySensors library](http://www.mysensors.org/). For more info
about the requirements, please read on.

## Hardware Prerequisites

The shetch is intended for Arduino compatible board [Devduino v2.0](http://www.seeedstudio.com/wiki/DevDuino_Sensor_Node_V2.0_(ATmega_328)). The board comes with MCP9700 chip, which is an analog temperature sensor. This sketch also uses digital temperature and humidity sensor [DHT22](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf) for maximum precision. The sensor node is wireless, and uses the [nRF24L01+ wireless module](https://www.sparkfun.com/datasheets/Wireless/Nordic/nRF24L01P_Product_Specification_1_0.pdf) for communication with sensor gateway.

### Complete List

* [Devduino v2.0 board](http://www.seeedstudio.com/wiki/DevDuino_Sensor_Node_V2.0_(ATmega_328))
* [nRF24L01+ wireless module](https://www.sparkfun.com/datasheets/Wireless/Nordic/nRF24L01P_Product_Specification_1_0.pdf)
* [DHT22 sensor](http://www.seeedstudio.com/depot/Grove-TemperatureHumidity-Sensor-Pro-p-838.html) [(datasheet)](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf)

## Software Prerequisites a.k.a. Libraries

For communication with the DHT22 sensor the [arduino-DHT library from markruys](https://github.com/markruys/arduino-DHT) is used. Note that there are many versions of different DHT libraries. If you want the sketch to work out-of-the-box, use the same library. As wireless communication protocol library, the [MySensors library](http://www.mysensors.org/) is used.

### Complete List

* [MySensors library](http://www.mysensors.org/)
  * For [**master** branch of devduino-temp-hum-sensor](https://github.com/thozza/devduino-temp-hum-sensor/tree/master) use the [**development version** of MySensors library](https://github.com/mysensors/Arduino/tree/development )
  * If you want to use the [**version 1.4** of MySensors library (latest stable)](https://github.com/mysensors/Arduino/tree/master) use the [**mysensors-protocol-1.4** branch of devduino-temp-hum-sensor](https://github.com/thozza/devduino-temp-hum-sensor/tree/mysensors-protocol-v1.4)
* [arduino-DHT library from markruys](https://github.com/markruys/arduino-DHT)

## How it works?

The sketch has the **node ID** hardcoded to **1**. If you want to use different **node ID** or want to automatically request the **node ID**, redefine the ```NODE_ID``` in the sketch. The shetch also uses the default settings of [MySensors library](http://www.mysensors.org/) for the **nRF24L01+** module (like channel, etc.). The sensor node goes to sleep for **5 seconds** after sending all values to the gateway. To change this, redefine the ```SLEEP_TIME``` to different value in **milliseconds**.

### Sensor Setup Phase
1. Initialization of the sensor node
2. Send the sketch info to the gateway
3. Present all connected sensors
  * Sensor ID **0** - DHT22 sensor humidity
  * Sensor ID **1** - DHT22 sensor temperature
  * Sensor ID **2** - MCP9700 sensor temperature

### Sensor Execution Phase
* Each **5 seconds** do the following:

1. send battery level in percents
2. send DHT22 sensor humidity
3. DHT22 sensor temperature
4. MCP9700 sensor temperature
5. go to sleep
