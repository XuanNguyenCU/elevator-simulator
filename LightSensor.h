#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include "Sensor.h"

class LightSensor : public Sensor {

private:
    int triggerCount;      // Number of times the sensor is triggered

public:
    LightSensor();
    bool update() override;
    bool isRepeatedlyTriggered() const;
};

#endif
