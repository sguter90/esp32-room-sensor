# Integrating into Homebridge

1. Install [Mqttthing](https://github.com/arachnetech/homebridge-mqttthing) plugin in [Homebridge](https://github.com/homebridge/homebridge)
1. Configure device (Add entry to "accessories" array) and restart homebridge
```json
{
    "accessories": [
        {
            "accessory": "mqttthing",
            "type": "airQualitySensor",
            "name": "living room",
            "topics": {
                "getAirQuality": "LivingRoom/RoomSensor/quality",
                "getCurrentTemperature": "LivingRoom/RoomSensor/temperature",
                "getCurrentRelativeHumidity": "LivingRoom/RoomSensor/humidity"
            },
            "history": true
        }
    ]
}
```

## Apple Home
![Apple Home App](images/apple-home.jpg)

## Eve
![Eve Overview](images/eve-overview.jpg)
![Eve Overview](images/eve-detail.jpg)
