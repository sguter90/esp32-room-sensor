#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <string>

#ifdef ESP8266
#include <ESP8266WiFi.h>
typedef ESP8266WiFiClass WiFiClass;
#else
#include <WiFi.h>
#endif

void handleWifiEvent(WiFiEvent_t event)
{
  Serial.printf("[WiFi-event] event: %d\n", event);
  #ifdef ESP8266
  #else 
  switch (event)
  {
  case SYSTEM_EVENT_WIFI_READY:
    Serial.println("WiFi interface ready");
    break;
  case SYSTEM_EVENT_SCAN_DONE:
    Serial.println("Completed scan for access points");
    break;
  case SYSTEM_EVENT_STA_START:
    Serial.println("WiFi client started");
    break;
  case SYSTEM_EVENT_STA_STOP:
    Serial.println("WiFi clients stopped");
    break;
  case SYSTEM_EVENT_STA_CONNECTED:
    Serial.println("Connected to access point");
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("Disconnected from WiFi access point");
    break;
  case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
    Serial.println("Authentication mode of access point has changed");
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.printf("Obtained IP address: %s", WiFi.localIP());
    break;
  case SYSTEM_EVENT_STA_LOST_IP:
    Serial.println("Lost IP address and IP address is reset to 0");
    //      vTaskDelay( 5000 );
    //      ESP.restart();
    break;
  case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
    Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
    break;
  case SYSTEM_EVENT_STA_WPS_ER_FAILED:
    Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
    //      ESP.restart();
    break;
  case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
    Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
    break;
  case SYSTEM_EVENT_STA_WPS_ER_PIN:
    Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
    break;
  case SYSTEM_EVENT_AP_START:
    Serial.println("WiFi access point started");
    break;
  case SYSTEM_EVENT_AP_STOP:
    Serial.println("WiFi access point  stopped");
    //      WiFi.mode( WIFI_OFF);
    //      esp_sleep_enable_timer_wakeup( 1000000 * 2 ); // 1 second times how many seconds wanted
    //      esp_deep_sleep_start();
    break;
  case SYSTEM_EVENT_AP_STACONNECTED:
    Serial.println("Client connected");
    break;
  case SYSTEM_EVENT_AP_STADISCONNECTED:
    Serial.println("WiFi client disconnected");
    break;
  case SYSTEM_EVENT_AP_STAIPASSIGNED:
    Serial.println("Assigned IP address to client");
    break;
  case SYSTEM_EVENT_AP_PROBEREQRECVED:
    Serial.println("Received probe request");
    break;
  case SYSTEM_EVENT_GOT_IP6:
    Serial.println("IPv6 is preferred");
    break;
  case SYSTEM_EVENT_ETH_GOT_IP:
    Serial.println("Obtained IP address");
    break;
  default:
    break;
  }
  #endif
}

void setupWiFi(WiFiClass &connection, std::string ssid, std::string password, Print &output)
{
  output.print("Connecting to WiFi: ");
  output.println(ssid.c_str());
  // connection.onEvent(handleWifiEvent); // activate for debugging
  connection.begin(ssid.c_str(), password.c_str());
  uint32_t notConnectedCounter = 0;
  while (connection.status() != WL_CONNECTED)
  {
    output.print(".");
    notConnectedCounter++;
    if (notConnectedCounter > 150)
    {
      output.println();
      output.println("Resetting due to Wifi not connecting...");
      WiFi.disconnect();
      delay(1000);
      ESP.restart();
    }
    delay(100);
  }

  output.println("");
  output.println("WiFi connected!");
  output.print("Got IP: ");
  output.println(connection.localIP().toString().c_str());
}

void keepWiFiConnected(WiFiClass &connection, int checkIntervalSeconds, long &lastCheckMillis, Print &output)
{
  unsigned long currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - lastCheckMillis >= checkIntervalSeconds))
  {
    output.print(millis());
    output.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    lastCheckMillis = currentMillis;
  }
}

#endif