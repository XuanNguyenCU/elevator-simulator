#include "Elevator.h"
#include "MainWindow.h"

Elevator::Elevator(int elevatorID) : QObject(), 
    elevatorID(elevatorID), 
    currentFloor(0), 
    direction(0), 
    numPassengers(0), 
    backupBattery(false), 
    outOfService(false), 
    controlPanel(new ControlPanel(this)) {
    connect(&moveTimer, &QTimer::timeout, this, &Elevator::processMove);
    connect(&doorTimer, &QTimer::timeout, this, &Elevator::closeDoor);
    moveTimer.setSingleShot(true);
    doorTimer.setSingleShot(true);
}

Elevator::~Elevator() {
    delete controlPanel;
}

bool Elevator::addSelectedFloor(int floor) {
    if (outOfService) {
        return false;
    }
    selectedFloors.push(floor);
    return true;
}

void Elevator::enterElevator() {
    numPassengers++;
    displayPassengerCount();
}

void Elevator::exitElevator() {
    if (numPassengers > 0) {
        numPassengers--;
    }
    displayPassengerCount();
}

void Elevator::closeDoor() {
    door.close();

    displayUpdate(currentFloor, direction);

    if (lightSensor.update()) {
        handleObstacles();
        return;
    }
    if (weightSensor.update()) {
        handleOverload();
        return;
    }

    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + ": Door CLOSED");
    if (!selectedFloors.empty()) {
        move();
    } 
    
    else {
        ECS::getInstance()->update(true);
    }
}

void Elevator::openDoor() {
    door.open();

    displayUpdate(currentFloor, direction);
    doorTimer.start(DEFAULT_COUNTDOWN);
    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + ": Door OPEN");
}

void Elevator::halt() {
    direction = IDLE;

    displayUpdate(currentFloor, direction);

    if (outOfService) {
        ringDoorbell();
        displayAlert("SAFE FLOOR REACHED.");
        audioAlert(SAFEFLOOR);
        openDoor();
        doorTimer.stop();
        while (numPassengers > 0) exitElevator(); // all the passengers exit
    } 
    
    else {
        ECS::getInstance()->getFloorPanel(currentFloor)->reset();
        ringDoorbell();
        openDoor();
    }
}

void Elevator::move() {
    if (selectedFloors.empty()) return;
    int targetFloor = selectedFloors.front();
    // If the elevator is at the target floor, then remove current target floor from queue
    if (targetFloor == currentFloor) {
        selectedFloors.pop();
        halt();
        return;
    }
    direction = (targetFloor > currentFloor) ? UP : DOWN;
    moveTimer.start(MOVE_DURATION);
}

void Elevator::processMove() {
    currentFloor += direction;

    displayUpdate(currentFloor, direction);

    // Update the floor sensor to the current floor
    floorSensor.updateFloor(currentFloor);

    // Check if the elevator has reached the target floor using FloorSensor
    if (floorSensor.isCorrectFloor(selectedFloors.front())) {
        selectedFloors.pop();
        halt();
    } 
    
    else {
        move();
    }
}

void Elevator::moveToSafeFloor() {
    moveTimer.stop();
    outOfService = true;
    selectedFloors = std::queue<int>();
    selectedFloors.push(ECS::getInstance()->getSafeFloor());
    doorTimer.stop();
    closeDoor();
}

void Elevator::handleObstacles() {
    if (!door.isOpened()) {
        ringDoorbell();
        openDoor();
        doorTimer.stop();
    }

    QTimer::singleShot(SENSOR_TICK, this, [this]() {
        if (!lightSensor.update()) {
            doorTimer.stop();
            closeDoor();
        } 
        
        else if (lightSensor.isRepeatedlyTriggered()) {
            handleObstacles();
            displayAlert("Door is repeatedly obstructed!");
            audioAlert(DOOROBSTACLE);
        } 
        
        else {
            handleObstacles();
        }
    });
}

void Elevator::handleHelpSignal() {
    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + " audio system: " + HELP);

    QTimer::singleShot(HELP_RESPONSE_TIME, this, [this]() {
        MainWindow* mainWindow = MainWindow::getMainWindow();
        bool buildingSafetyResponse = mainWindow->findChild<QCheckBox*>("toggleBuildingSafety")->isChecked();

        if (buildingSafetyResponse) {
            connectToSafetyServices();
        } 
        
        else {
            connectTo911();
        }
    });
}

void Elevator::handleOverload() {
    if (!door.isOpened()) {
        ringDoorbell();
        openDoor();
        doorTimer.stop();
    }

    displayAlert("Max weight exceeded!");
    audioAlert(OVERLOAD);
    
    QTimer::singleShot(SENSOR_TICK, this, [this]() {
        if (!weightSensor.update()) {
            doorTimer.stop();
            closeDoor();
        } else {
            handleOverload();
        }
    });
}

void Elevator::handleFireAlarm() {
    displayAlert("Fire in the building!");
    audioAlert(FIREALARM);
    moveToSafeFloor();
}

void Elevator::handlePowerOutage() {
    switchToBattery();
    displayAlert("Power Outage! Switching to backup battery.");
    audioAlert(POWEROUTAGE);
    moveToSafeFloor();
}

void Elevator::switchToBattery() {
    backupBattery = true;
}

void Elevator::connectToSafetyServices() const { 
    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + " Display: Connecting to Building Safety Services...");
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + " received building safety reply.");
}

// If safety services did not respond
void Elevator::connectTo911() const { 
    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + " Display: Connecting to 911...");
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + " made emergency call.");
} 

void Elevator::ringDoorbell() {
    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + ": DING!");
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + " has reached Floor " + std::to_string(currentFloor) + ".");
}

void Elevator::audioAlert(const std::string& message) const { 
    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + " Audio: " + message);
}

void Elevator::displayAlert(const std::string& message) const { 
    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + " Display: " + message);
}

void Elevator::displayUpdate(int floor, int direction) const { 
    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->updateDisplay(elevatorID, floor, direction);
}

void Elevator::displayPassengerCount() const {
    MainWindow* mainWindow = MainWindow::getMainWindow();
    mainWindow->addEventLog("Elevator " + std::to_string(elevatorID) + " has " + std::to_string(numPassengers) + " passengers.");
}