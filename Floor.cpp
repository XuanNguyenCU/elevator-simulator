#include "Floor.h"

Floor::Floor(int floorNum) : floorNum(floorNum), upButtonIlluminated(false), downButtonIlluminated(false) {}

void Floor::pressButton(int direction) {
    MainWindow* mainWindow = MainWindow::getMainWindow();

    if (direction == UP) {
        upButtonIlluminated = true;
        mainWindow->addEventLog("UP button is illuminated on Floor " + std::to_string(floorNum));
    } 
    
    else {
        downButtonIlluminated = true;
        mainWindow->addEventLog("DOWN button is illuminated on Floor " + std::to_string(floorNum));
    }
    ECS::getInstance()->callElevatorToPassenger(floorNum);
}

void Floor::reset() {
    MainWindow* mainWindow = MainWindow::getMainWindow();
    if (upButtonIlluminated) {
        upButtonIlluminated = false;
        mainWindow->addEventLog("Up button reset on Floor " + std::to_string(floorNum));
    }

    if (downButtonIlluminated) {
        downButtonIlluminated = false;
        mainWindow->addEventLog("Down button reset on Floor " + std::to_string(floorNum));
    }
}
