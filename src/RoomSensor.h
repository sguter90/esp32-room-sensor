#ifndef REMOTE_ROOMSENSOR_H
#define REMOTE_ROOMSENSOR_H

#include <string>
#include <stdint.h>
#include "bsec.h"
#include <sys/time.h>

class SensorData
{
public:
    float temperature;         // °C
    uint32_t pressure;         // hPa
    float humidity;            // %
    float gasResistance;       // Ohm - raw value for calculating gas values
    float co2Equivalent;       // CO2eq - ppm scale
    float breathVocEquivalent; // bVOCeq - ppm scale

    // Indoor air quality (0 - 500)
    // 0   -  50 = Excellent
    // 51  - 100 = Good
    // 101 - 150 = Lightly polluted
    // 151 - 200 = Moderately polluted
    // 201 - 250 = Heavily polluted
    // 251 - 350 = Severely polluted
    // > 351     = Extremely polluted
    float iaq;

    // 0 = just started / stabilizing or timing violation
    // 1 = history is uncertain - data too stable to clearly define references
    // 2 = currently calibrating
    // 3 = calibrated successfully
    uint8_t iaqAccuracy;
    uint8_t co2Accuracy;
    uint8_t breathVocAccuracy;

    std::string getAirQualityState();
};

typedef std::function<void(SensorData data)> SensorDataEvent;

class RoomSensor
{
private:
    Bsec *_sensor;
    Print *_output;
    SensorDataEvent _cbEvent;
    SensorData _data;
    int _nextReading = 0;
    int _readIntervall = 5000; // ms

public:
    RoomSensor();
    RoomSensor(Bsec &sensor, Print &output);
    void onEvent(SensorDataEvent cbEvent);
    void readData();
    void setReadIntervall(int milliSeconds);
    void printTemperature(SensorData data);
    void printPressure(SensorData data);
    void printHumidity(SensorData data);
    void printIAQ(SensorData data);
    void printCo2Equivalent(SensorData data);
    void printBreathVocEquivalent(SensorData data);
    void printAccuracy(SensorData data);
    void printSensorData(SensorData data);
    void printSensorStatus();
    void printSensorState(const uint8_t* state);

    uint8_t sensorState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
    int64_t sensorStateTime = 0;
};

#endif