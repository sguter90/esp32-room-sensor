#include "MQTTSetup.h"

MQTTSetup::MQTTSetup()
{
}

MQTTSetup::MQTTSetup(PubSubClient &client, MQTTConfig &config, Print &output)
{
    _client = &client;
    _config = &config;
    _printer = &output;

    const char* serverName = _config->server.c_str();
    _client->setServer(serverName, _config->port);
}

void MQTTSetup::connect()
{
    int connectionFails = 0;
    MQTTConfig *cfg = this->_config;
    const char* serverName = _config->server.c_str();

    while (!this->_client->connected())
    {
        this->_printer->print("Attempting MQTT connection to: ");
        this->_printer->print(serverName);
        this->_printer->print(":");
        this->_printer->print(cfg->port);
        this->_printer->print(" as ");
        this->_printer->println(cfg->thingName.c_str());

        // MQTT connection: Attempt to connect to MQTT broker 3 times: SUCCESS -> continue | FAILED restart ESP
        // On restart it will first try to connect to the previously set AP. If that fails the config portal will be started.
        // If the config portal is not used within wifiTimeout (set in portal), the ESP will retry connecting to the previously set AP again.
        if (this->_client->connect(cfg->thingName.c_str(), cfg->user.c_str(), cfg->pass.c_str()))
        {
            this->_printer->println("Connected to MQTT broker");
            connectionFails = 0;
        }
        else
        {
            if (++connectionFails == 3)
            {
                this->_printer->println("MQTT broker connection timeout...restarting!");
                delay(1000);
                ESP.restart();
                delay(5000);
                break;
            }
            this->_printer->println("Failed to connenct - try again in 5 seconds...");
            delay(5000);
        }
    }
}

void MQTTSetup::keepConnection()
{
    if (this->_client->connected())
    {
        this->_client->loop();
        return;
    }

    this->_printer->println("MQTT broker disconnected - connecting...");
    this->connect();
}

const std::string MQTTSetup::getTopic(std::string topic)
{
    std::string prefix = this->_config->topicPrefix;
    std::string result = prefix + topic;

    return result;
}