#ifndef FLOORSENSOR_H
#define FLOORSENSOR_H

#include "Sensor.h"

class FloorSensor : public Sensor  {
    
private:
    int currentFloor;  // The current detected floor of the elevator

public:
    explicit FloorSensor(int initialFloor = 0) : currentFloor(initialFloor) {}

    // Updates the sensor with the elevator's current floor position
    bool update() override {return true;}
    void updateFloor(int newFloor) { currentFloor = newFloor; }

    // Checks if the elevator has reached the specified target floor
    bool isCorrectFloor(int targetFloor) const { return currentFloor == targetFloor; }
};

#endif // FLOORSENSOR_H
