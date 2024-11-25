#include "ControlPanel.h"
#include "Elevator.h"

ControlPanel::ControlPanel(Elevator* elevator) : elevator(elevator) {}

void ControlPanel::chooseFloorButton(int floorNum) const {
    elevator->addSelectedFloor(floorNum);
}

void ControlPanel::pressOpenDoor() const {
    elevator->openDoor();
}

void ControlPanel::pressCloseDoor() const {
    elevator->closeDoor();
}

void ControlPanel::pressHelpButton() const {
    elevator->handleHelpSignal();
}
