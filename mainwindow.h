#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QLCDNumber>
#include <QTimer>
#include "ECS.h"

class Elevator;
class Floor;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static MainWindow* getMainWindow() { return singleton; }
    int getNumFloors() const { return numFloors; }
    int getNumElevators() const { return numElevators; }
    int getSafeFloor() const { return safeFloor; }
    void addEventLog(const std::string& event);
    void clearLayout(QLayout* layout);
    void updateDisplay(int elevatorID, int floor, int direction);

public slots:
    void incrementSimulationTime();

private:
    static MainWindow* singleton;
    Ui::MainWindow *ui;
    ECS* ecs;
    int numFloors;
    int numElevators;
    int safeFloor;
    int simulationTime;   // Variable to store current simulation time
    QTimer simulationTimer;   // Timer to track simulation time
    QLCDNumber *simulationTimeLabel;

private slots:
    void startSimulationHandler();
    void passengerEnterHandler();
    void passengerExitHandler();
    void numFloorsSpinHandler();
    void fromFloorSpinHandler();
    void elevatorSelectionHandler();
    void upButtonHandler();
    void downButtonHandler();
    void targetFloorButtonHandler();
    void helpButtonHandler();
    void fireAlarmButtonHandler();
    void localFireAlarmHandler();
    void outageButtonHandler();
};

#endif
