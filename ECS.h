#ifndef ECS_H
#define ECS_H
#include "defs.h"

class Elevator;
class Floor;

class ECS {

public:
    ~ECS();
    static ECS* getInstance();
    static ECS* newInstance();

    int getNumFloors() const { return numFloors; }
    int getSafeFloor() const { return safeFloor; }
    Elevator* getElevator(int i) const { return elevators[i]; }
    Floor* getFloorPanel(int i) const { return floorPanels[i]; }
    void update(bool isFree);
    void callElevatorToPassenger(int floor);
    void fireAlarm();
    void powerOutage();

    static ECS* singleton;

private:
    ECS(int numFloors, int numElevators, int safeFloor);
    int numElevators;
    int numFloors;
    int safeFloor;
    Floor** floorPanels;
    Elevator** elevators;
    std::queue<int> queuedRequests;
};

#endif
