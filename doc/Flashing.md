# Flash ESP32

## Copy and adjust configuration
To allow faster flashing of multiple devices all configuration variables are separated into a config header file.  
You can copy ``ConfigRoom.h.dist`` to something like ``ConfigLivingRoom.h`` and adjust names etc.  
After that you just need to include the configuration you like before flashing.

## Configuration parameters

| Parameter           | Description                                                      |
|---------------------|------------------------------------------------------------------|
| wifi_ssid           | SSID (name) of your WiFi                                         |
| wifi_password       | password for your WiFi                                           |
| wifiCheckInterval   | how often to check Wifi connection (and reconnect)               |
| mqttThingName       | name of your device                                              |
| mqttServer          | hostname/IP of the MQTT broker                                   |
| mqttPort            | port of the MQTT broker                                          |
| mqttUser            | username for MQTT broker                                         |
| mqttPass            | passwort for MQTT broker                                         |
| mqttTopicPrefix     | topic prefix which is prepanded for each sensor value            |
| temperatureOffset   | offset for temperature in degrees (if not calibrated by factory) |
| sensorReadIntervall | how often to submit data via MQTT and log output                 |

## Flash process

1. Open this repository via PIO Home (Platform IO Extension vor VSCode).
1. Connect the ESP32 via USB cable to your computer
1. In the PlatformIO section in "Project tasks" select "Upload" (or "Upload and Monitor")