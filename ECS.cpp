#include "ECS.h"
#include "MainWindow.h"
#include "Elevator.h"
#include "Floor.h"
#include "FloorSensor.h"

ECS* ECS::singleton = nullptr;

ECS::ECS(int numFloors, int numElevators, int safeFloor) : numFloors(numFloors), numElevators(numElevators), safeFloor(safeFloor) {
    floorPanels = new Floor*[numFloors];
    for (int i = 0; i < numFloors; i++) {
        floorPanels[i] = new Floor(i);
    }

    elevators = new Elevator*[numElevators];
    for (int i = 0; i < numElevators; i++) {
        elevators[i] = new Elevator(i);
    }
}

ECS::~ECS() {
    for (int i = 0; i < numFloors; i++) {
        delete floorPanels[i];
    }

    for (int i = 0; i < numElevators; i++) {
        delete elevators[i];
    }

    delete[] floorPanels;
    delete[] elevators;
    singleton = nullptr;
}

ECS* ECS::getInstance() {
    return singleton;
}

ECS* ECS::newInstance() {
    // Getter tO create the instance, called when starting the elevator simulation
    if (singleton != nullptr) {
        delete singleton;
    }

    MainWindow* mainWindow = MainWindow::getMainWindow();
    singleton = new ECS(mainWindow->getNumFloors(), mainWindow->getNumElevators(), mainWindow->getSafeFloor());
    return singleton;
}

void ECS::callElevatorToPassenger(int floor) {
    for (int i = 0; i < numElevators; i++) {
        if (elevators[i]->isIdle()) {
            if (elevators[i]->addSelectedFloor(floor)) {
                elevators[i]->move();
                return;
            }
        }
    }
    queuedRequests.push(floor);
}

void ECS::fireAlarm() {
    for (int i = 0; i < numElevators; i++) {
        elevators[i]->handleFireAlarm();
    }
}

void ECS::powerOutage() {
    for (int i = 0; i < numElevators; i++) {
        elevators[i]->handlePowerOutage();
    }
}

void ECS::update(bool isFree) {

    if (isFree && !queuedRequests.empty()) {
        callElevatorToPassenger(queuedRequests.front());
        queuedRequests.pop();
    }
}
