#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QObject>
#include <QTimer>
#include <QCheckBox>
#include "defs.h"
#include "ControlPanel.h"
#include "Doors.h"
#include "Floor.h"
#include "FloorSensor.h"
#include "LightSensor.h"
#include "WeightSensor.h"

class Elevator : public QObject {
    Q_OBJECT

public:
    explicit Elevator(int id);
    ~Elevator() override;
    bool isEmpty() const {return numPassengers == 0;};
    bool isDoorOpen() const { return door.isOpened(); }
    bool isIdle() const {return selectedFloors.empty() && direction == IDLE && door.isOpened() == false;};
    bool addSelectedFloor(int floor);
    void enterElevator();    // Passenger enters the elevator
    void exitElevator();     // Passenger exits the elevator
    void closeDoor();
    void openDoor();
    void halt();
    void move();
    void processMove();
    void moveToSafeFloor();
    void handleObstacles();
    void handleHelpSignal();
    void handleOverload();
    void handleFireAlarm();
    void handlePowerOutage();
    void switchToBattery();
    void connectToSafetyServices() const;
    void connectTo911() const;
    void ringDoorbell();
    void audioAlert(const std::string& message) const;
    void displayAlert(const std::string& message) const;
    void displayUpdate(int floor, int direction) const;
    void displayPassengerCount() const;
    ControlPanel* getControlPanel() const {return controlPanel;};
    std::queue<int>* getQueuedFloors() {return &selectedFloors;};

private:
    int elevatorID;
    int currentFloor;
    int direction;
    int numPassengers;
    bool backupBattery;
    bool outOfService;
    std::queue<int> selectedFloors;
    QTimer moveTimer;
    QTimer doorTimer;
    Doors door;
    FloorSensor floorSensor;
    LightSensor lightSensor;
    WeightSensor weightSensor;
    ControlPanel* controlPanel;
};

#endif
