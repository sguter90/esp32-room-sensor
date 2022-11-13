#include <WiFiSetup.h>
#include "WebServerAdvanced.h"
#include <Wire.h>
#include <SPI.h>
#include "StringTools.h"
#include "RemoteLogging.h"
#include <PubSubClient.h>
#include "MQTTSetup.h"
#include "RoomSensor.h"
#include "ConfigMyRoom.h" // copy and adjust ConfigRoom.h.dist

// WIFI
long lastWifiCheck;

// webserver
WebServerAdvanced server(80);
WebSocketsServer webSocket(81);
#ifdef ESP8266
typedef ESP8266WebServer WebServer;
#endif

// MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);
MQTTSetup mqtt;
MQTTConfig cfg;

// BME680
Bsec sensor;
RoomSensor roomSensor;
SensorData sensorData;

// logging
RemoteLogging logger = RemoteLogging(server, webSocket);

// webserver page showing current sensor data
void http_handleIndex(WebServer &server, HTTPMethod requestMethod, String requestUri)
{
  std::string response = R"RESPONSE(
    <html>
    <head>
      <meta charset="UTF-8">
    </head>
    <body>
      <h1>Room sensor: {sensorName}</h1>
      <b>Temperature:</b> {temperature} &#xB0;C<br/>
      <b>Air pressure:</b> {pressure} hPa<br/>
      <b>Humidity:</b> {humidity} %<br/>
      <b>IAQ:</b> {iaq}<br/>
    </body>
    </html>
  )RESPONSE";

  replace(response, "{sensorName}", mqttThingName);
  replace(response, "{temperature}", std::to_string(sensorData.temperature));
  replace(response, "{pressure}", std::to_string(sensorData.pressure));
  replace(response, "{humidity}", std::to_string(sensorData.humidity));
  replace(response, "{iaq}", std::to_string(sensorData.iaq) + " (" + sensorData.getAirQualityState() + ")");

  char *cstr = new char[response.length() + 1];
  strcpy(cstr, response.c_str());

  server.send(200, "text/html", String(cstr));
}

// gets executed every time a websocket event is triggered
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  // log webpage uses a ping request to identify if connection is still up
  logger.pong(num, type, payload, length);
}

// gets executed every time the sensor delivers data
void sensorDataEvent(SensorData data)
{
  sensorData = data;
  roomSensor.printSensorData(data);
  logger.println();

  std::string temperatureTopic = mqtt.getTopic("temperature");
  std::string pressureTopic = mqtt.getTopic("pressure");
  std::string humidityTopic = mqtt.getTopic("humidity");
  std::string iaqTopic = mqtt.getTopic("iaq");
  std::string qualityTopic = mqtt.getTopic("quality");

  mqttClient.publish(temperatureTopic.c_str(), std::to_string(data.temperature).c_str());
  mqttClient.publish(pressureTopic.c_str(), std::to_string(data.pressure).c_str());
  mqttClient.publish(humidityTopic.c_str(), std::to_string(data.humidity).c_str());
  mqttClient.publish(iaqTopic.c_str(), std::to_string(data.iaq).c_str());
  mqttClient.publish(qualityTopic.c_str(), data.getAirQualityState().c_str());
}

// setup and register additional serial log to remote logger
void setupLogging()
{
  Serial.begin(115200);
  delay(200);
  logger.setBufferSize(10);
  logger.registerSerialLog(Serial);
}

// setup routes and wep pages and start servers
void setupWebserver()
{
  server.on("/", HTTP_GET, http_handleIndex); // show sensor data
  logger.registerHttpLogHandler("/log");      // view lines printed via logger

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  logger.println("HTTP server started");
}

// connect MQTT broker
void setupMqtt()
{
  cfg.thingName = mqttThingName;
  cfg.server = mqttServer;
  cfg.port = mqttPort;
  cfg.user = mqttUser;
  cfg.pass = mqttPass;
  cfg.topicPrefix = mqttTopicPrefix;

  mqtt = MQTTSetup(mqttClient, cfg, logger);

  mqtt.connect();
}

// setup BME680 parameters and register it at RoomSensor helper class
void setupRoomSensor()
{
  Wire.begin();
  sensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
  sensor.setTemperatureOffset(temperatureOffset);
  roomSensor = RoomSensor(sensor, logger);
  roomSensor.setReadIntervall(sensorReadIntervall);
  roomSensor.printSensorStatus();

  bsec_virtual_sensor_t sensorList[10] = {
      BSEC_OUTPUT_RAW_TEMPERATURE,
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_HUMIDITY,
      BSEC_OUTPUT_RAW_GAS,
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
      BSEC_OUTPUT_CO2_EQUIVALENT,
      BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  sensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  roomSensor.printSensorStatus();
  roomSensor.onEvent(sensorDataEvent);
}

void setup()
{
  delay(1000);
  setupLogging();
  setupWiFi(WiFi, wifi_ssid, wifi_password, logger);
  setupWebserver();
  setupMqtt();
  setupRoomSensor();
}

void loop()
{
  keepWiFiConnected(WiFi, wifiCheckInterval, lastWifiCheck, logger);
  mqtt.keepConnection();
  roomSensor.readData();
  server.handleClient();
  webSocket.loop();
}