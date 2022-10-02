#include "RoomSensor.h"

std::string SensorData::getAirQualityState()
{
    if (this->iaq <= 50) {
        return "EXCELLENT";
    } else if(this->iaq <= 150) {
        return "GOOD";
    } else if(this->iaq <= 200) {
        return "FAIR";
    } else if(this->iaq <= 200) {
        return "INFERIOR";
    } else if(this->iaq > 200) {
        return "POOR";
    }

    return "UNKNOWN";
}

RoomSensor::RoomSensor()
{
}

RoomSensor::RoomSensor(Bsec &sensor, Print &output)
{
    _sensor = &sensor;
    _output = &output;
}

void RoomSensor::onEvent(SensorDataEvent cbEvent)
{
    _cbEvent = cbEvent;
}

void RoomSensor::readData()
{
    // always run sensor but only process data in read intervall
    if (this->_sensor->run())
    {
        if (millis() < this->_nextReading)
        {
            return;
        }

        this->_nextReading = millis() + this->_readIntervall;

        this->_data.temperature = this->_sensor->temperature;
        this->_data.pressure = this->_sensor->pressure / 100.0;
        this->_data.humidity = this->_sensor->humidity;
        this->_data.iaq = this->_sensor->iaq;
        this->_data.iaqAccuracy = this->_sensor->iaqAccuracy;
        this->_data.gasResistance = this->_sensor->gasResistance;
        this->_data.co2Equivalent = this->_sensor->co2Equivalent;
        this->_data.co2Accuracy = this->_sensor->co2Accuracy;
        this->_data.breathVocEquivalent = this->_sensor->breathVocEquivalent;
        this->_data.breathVocAccuracy = this->_sensor->breathVocAccuracy;

        if (this->_cbEvent)
        {
            this->_cbEvent(this->_data);
        }
    }
    else
    {
        this->printSensorStatus();
    }
}

void RoomSensor::setReadIntervall(int milliSeconds)
{
    this->_readIntervall = milliSeconds;
}

void RoomSensor::printTemperature(SensorData data)
{
    this->_output->print(F("Temperature = "));
    this->_output->print(data.temperature);
    this->_output->println(F(" °C"));
}

void RoomSensor::printPressure(SensorData data)
{
    this->_output->print(F("Pressure = "));
    this->_output->print(data.pressure);
    this->_output->println(F(" hPa"));
}

void RoomSensor::printHumidity(SensorData data)
{
    this->_output->print(F("Humidity = "));
    this->_output->print(data.humidity);
    this->_output->println(F(" %"));
}

void RoomSensor::printIAQ(SensorData data)
{
    this->_output->print(F("IAQ = "));
    this->_output->print(data.iaq);
    this->_output->print(" (");
    this->_output->print(data.getAirQualityState().c_str());
    this->_output->println(")");
}

void RoomSensor::printCo2Equivalent(SensorData data)
{
    this->_output->print(F("CO2 Equivalent = "));
    this->_output->print(data.co2Equivalent);
    this->_output->println(F(" ppm"));
}

void RoomSensor::printBreathVocEquivalent(SensorData data)
{
    this->_output->print(F("Breath Voc Equivalent = "));
    this->_output->print(data.breathVocEquivalent);
    this->_output->println(F(" ppm"));
}

void RoomSensor::printAccuracy(SensorData data)
{
    this->_output->print(F("iaqAccuracy = "));
    this->_output->print(data.iaqAccuracy);
    this->_output->print(F(", "));
    this->_output->print(F("co2Accuracy = "));
    this->_output->print(data.co2Accuracy);
    this->_output->print(F(", "));
    this->_output->print(F("breathVocAccuracy = "));
    this->_output->print(data.breathVocAccuracy);
    this->_output->println();
}

void RoomSensor::printSensorData(SensorData data)
{
    this->printTemperature(data);
    this->printPressure(data);
    this->printHumidity(data);
    this->printIAQ(data);
    this->printCo2Equivalent(data);
    this->printBreathVocEquivalent(data);
    this->printAccuracy(data);
}

void RoomSensor::printSensorStatus()
{
    if (this->_sensor->status != BSEC_OK)
    {
        if (this->_sensor->status < BSEC_OK)
        {
            this->_output->println("BSEC error code : " + String(this->_sensor->status));
        }
        else
        {
            this->_output->println("BSEC warning code : " + String(this->_sensor->status));
        }
    }

    if (this->_sensor->bme680Status != BME680_OK)
    {
        if (this->_sensor->bme680Status < BME680_OK)
        {
            this->_output->println("BME680 error code : " + String(this->_sensor->bme680Status));
        }
        else
        {
            this->_output->println("BME680 warning code : " + String(this->_sensor->bme680Status));
        }
    }
}