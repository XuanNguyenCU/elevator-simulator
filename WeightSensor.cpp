#include "WeightSensor.h"

WeightSensor::WeightSensor() : Sensor() {}

bool WeightSensor::update() {
    MainWindow* mainWindow = MainWindow::getMainWindow();
    return mainWindow->QObject::findChild<QCheckBox*>("testOverload")->isChecked();
}
