#ifndef WEIGHTSENSOR_H
#define WEIGHTSENSOR_H

#include "Sensor.h"

class WeightSensor : public Sensor {
private:
    float currentWeight;  // The current detected weight
    float maxWeight;      // Maximum allowed weight in the elevator

public:
    WeightSensor();
    bool update() override;
    
};

#endif
