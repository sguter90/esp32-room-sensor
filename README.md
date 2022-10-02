# esp32-room-sensor

Uses the BME680 sensor to read temperature, pressure, humidity and IAQ and provides the data via MQTT and Web.

## What this code does

1. Connects to WiFi
1. Connects to MQTT broker
1. Reads the sensor data of the BME680
1. Runs a webserver which provides two routes:
1.1. http://<IP address>/ -> provides a way to view current sensor data
1.1. http://<IP address>/log -> allows to watch everything printed even if the ESP32 isn't connected to Serial
1. Runs websockets to push everything logged to the "/log" webpage
1. The "/log" webpage additonally sends a ping request via JavaScript websocket and the ESP32 responds with a pong message to identify connection issues on client side.


## Hardware

### Main parts

* [ESP32 Dev Kit C V4 NodeMCU](https://www.amazon.de/gp/product/B07Z83H831)
* [BME680 Environmental sensor](https://de.aliexpress.com/item/4001113450307.html)

### Additional/Optional parts

#### Soldering
* [Mat](https://www.amazon.de/gp/product/B07BXTFWV9)
* [Bolt](https://www.amazon.de/gp/product/B07G8CMMW5)
* [Solder](https://www.amazon.de/gp/product/B000V8JYP8)

#### Connecting stuff
* [Breadboard](https://www.amazon.de/gp/product/B01MCRZFE5)
* [Jump wires](https://www.amazon.de/dp/B07PRGFW5Z)


## Software
* [platformio - for uploading sketches (VSCode recommended)](https://platformio.org/)
* MQTT broker like [Mosquitto](https://mosquitto.org/) or [hmq](https://github.com/fhmq/hmq)
* [MQTT explorer - for debugging](https://mqtt-explorer.com/)

# HowTo

1. Solder BME680 pin headers
1. [Connect ESP32 to BME680](doc/Wiring.md)
1. [Flash ESP32](doc/Flashing.md)

# Acknowledgments
Special thankts to:
* [BSEC Arduino Library by Bosch Sensortec](https://github.com/BoschSensortec/BSEC-Arduino-library)
* [pubsubclient by knolleary](https://github.com/knolleary/pubsubclient)
* [WebSocket Server and Client by Links2004](https://github.com/Links2004/arduinoWebSockets)
