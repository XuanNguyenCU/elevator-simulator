#include "LightSensor.h"

LightSensor::LightSensor() : Sensor(), triggerCount(0) {}

bool LightSensor::update() {

    MainWindow* mainWindow = MainWindow::getMainWindow();
    bool isTriggered = mainWindow->QObject::findChild<QCheckBox*>("testObstacle")->isChecked();
    
    if (isTriggered) {
        triggerCount++;
        if (!isRepeatedlyTriggered()) {
            mainWindow->addEventLog("Door Sensor is triggered!");
        }
    } 
    
    else {
        triggerCount = 0;
    }

    return isTriggered;
}

bool LightSensor::isRepeatedlyTriggered() const {
    return triggerCount > SENSOR_THRESHOLD;
}
