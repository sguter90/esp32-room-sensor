#ifndef MQTT_SETUP_H
#define MQTT_SETUP_H

#include <string>
#include <PubSubClient.h>

class MQTTConfig
{
public:
    std::string server;
    uint16_t port = 1883;
    std::string user;
    std::string pass;
    std::string thingName;
    std::string topicPrefix;
};

class MQTTSetup
{
private:
    PubSubClient *_client;
    MQTTConfig *_config;
    Print *_printer;

public:
    MQTTSetup();
    MQTTSetup(PubSubClient &client, MQTTConfig &config, Print &output);
    void connect();
    void keepConnection();
    const std::string getTopic(std::string topic);
};

#endif
