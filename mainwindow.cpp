#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Elevator.h"
#include "Floor.h"
#include "ControlPanel.h"

MainWindow* MainWindow::singleton = nullptr;

MainWindow::MainWindow(QWidget *parent): 
    QMainWindow(parent), 
    ui(new Ui::MainWindow) {

    ui->setupUi(this);
    ui->elevatorStatusFrame->setHidden(1);
    ui->currentElevatorFrame->setHidden(1);
    ui->controlPanelFrame->setHidden(1);
    ui->outageButton->setDisabled(1);
    ui->fireAlarmButton->setDisabled(1);

    connect(ui->rebuildButton, SIGNAL(released()), this, SLOT(startSimulationHandler()));
    connect(ui->outageButton, SIGNAL(clicked()), this, SLOT(outageButtonHandler()));
    connect(ui->fireAlarmButton, SIGNAL(clicked()), this, SLOT(fireAlarmButtonHandler()));
    connect(ui->fromFloorSpin, SIGNAL(valueChanged(int)), this, SLOT(fromFloorSpinHandler()));
    connect(ui->elevatorSelectionSpin, SIGNAL(valueChanged(int)), this, SLOT(elevatorSelectionHandler()));
    connect(ui->directionUp, SIGNAL(clicked()), this, SLOT(upButtonHandler()));
    connect(ui->directionDown, SIGNAL(clicked()), this, SLOT(downButtonHandler()));
    connect(ui->passengerEnterButton, SIGNAL(clicked()), this, SLOT(passengerEnterHandler()));
    connect(ui->passengerExitButton, SIGNAL(clicked()), this, SLOT(passengerExitHandler()));
    connect(ui->numFloorsSpin, SIGNAL(valueChanged(int)), this, SLOT(numFloorsSpinHandler()));
    connect(ui->localFireAlarmButton, SIGNAL(clicked()), this, SLOT(localFireAlarmHandler()));
    ecs = nullptr;
    singleton = this;
    ui->eventLog->appendPlainText(QString::fromStdString("Input an amount of elevators to be generated to start the simulation."));
}

MainWindow::~MainWindow() {
    delete ui;
    delete ecs;
}

void MainWindow::addEventLog(const std::string &event) {
    ui->eventLog->appendPlainText(QString::fromStdString(event));
}

void MainWindow::startSimulationHandler() {
    numFloors = ui->numFloorsSpin->value();
    numElevators = ui->numElevatorsSpin->value();
    safeFloor = ui->safeFloorSpin->value();
    
    ui->eventLog->clear();  // Clear previous event logs
    ui->elevatorStatusFrame->setHidden(false);
    ui->currentElevatorFrame->setHidden(false);
    ui->controlPanelFrame->setHidden(false);
    ui->outageButton->setEnabled(1);
    ui->fireAlarmButton->setEnabled(1);

    ecs = ECS::newInstance();
    // Clear previous buttons
    QGridLayout *controlPanelLayout = ui->controlPanelLayout;
    QLayoutItem *item;
    while ((item = controlPanelLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    int row = 0;
    int col = 0;

    // Generate buttons for each floor
    for (int i = 0; i < numFloors; ++i) {
        QPushButton *button = new QPushButton(QString::number(i), this);
        button->setProperty("floor", i);
        connect(button, &QPushButton::released, this, &MainWindow::targetFloorButtonHandler);
        controlPanelLayout->addWidget(button, row, col, 1, 1);

        col += 1;
        if (col == 3) {
            row += 1;
            col = 0;
        }
    }
    QPushButton *helpButton = new QPushButton("Help", this);
    connect(helpButton, &QPushButton::released, this, &MainWindow::helpButtonHandler);
    controlPanelLayout->addWidget(helpButton, row, col, 1, 1);

    // Elevator status display
    clearLayout(ui->elevatorStatusLayout);
    QVBoxLayout *elevatorStatusLayout = ui->elevatorStatusLayout;

    for (int i = 0; i < numElevators; ++i) {
        QHBoxLayout *hLayout = new QHBoxLayout();

        QFont labelFont;
        labelFont.setPointSize(13);
        labelFont.setBold(true);

        // Elevator number label
        QLabel *label = new QLabel(QString("Elevator %1").arg(i));
        label->setObjectName(QString("elevatorLabel_%1").arg(i));
        label->setFont(labelFont);
        label->setStyleSheet("color: #333; background-color: #ECECEC; padding: 6px; border-radius: 6px; margin-right: 10px;");
        hLayout->addWidget(label);

        // Elevator floor indicator
        QLCDNumber *lcdNumber = new QLCDNumber(3);
        lcdNumber->setObjectName(QString("elevatorLCD_%1").arg(i));
        lcdNumber->setSegmentStyle(QLCDNumber::Flat);
        lcdNumber->setStyleSheet("background-color: #3A3A60; color: #FFFFFF; border-radius: 6px;");
        lcdNumber->setFixedHeight(35);
        lcdNumber->display(0);  // Initial floor display
        hLayout->addWidget(lcdNumber);

        // Elevator status label (defaulting to IDLE)
        QLabel *statusLabel = new QLabel("IDLE");
        statusLabel->setObjectName(QString("elevatorStatusLabel_%1").arg(i));
        statusLabel->setFont(labelFont);
        statusLabel->setStyleSheet("color: #008000; padding: 6px; border-radius: 6px; background-color: #DFF2D8;");
        statusLabel->setFixedWidth(100);
        hLayout->addWidget(statusLabel);

        elevatorStatusLayout->addLayout(hLayout);
    }

    if (simulationTimer.isActive()) { 
        ui->eventLog->appendPlainText("Simulation ended at " + QString::number(simulationTime) + " seconds.");
        simulationTimer.stop(); // Stop any previous timer
    }

    // Any existing connections to incrementSimulationTime are disconnected
    disconnect(&simulationTimer, &QTimer::timeout, this, &MainWindow::incrementSimulationTime);

    // Set simulation time to 0 at the start
    simulationTime = 0;

    // Connect timer to incrementSimulationTime slot, if not already connected
    connect(&simulationTimer, &QTimer::timeout, this, &MainWindow::incrementSimulationTime);
    simulationTimer.start(1000);  // Start timer with a 1-second interval

    // Spacer item to push the timer to the bottom of the layout
    QSpacerItem *spacer = new QSpacerItem(20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding);
    elevatorStatusLayout->addItem(spacer);

    QHBoxLayout *timerLayout = new QHBoxLayout();
    QFont labelFont;
    labelFont.setPointSize(13);
    labelFont.setBold(true);

    QLabel *timerLabel = new QLabel("Simulation Time:");
    timerLabel->setFont(labelFont);
    timerLabel->setStyleSheet("color: #333; background-color: #ECECEC; padding: 6px; border-radius: 6px; margin-right: 10px;");
    timerLayout->addWidget(timerLabel);
    simulationTimeLabel = new QLCDNumber(3);
    simulationTimeLabel->setObjectName("simulationTimeLabel");
    simulationTimeLabel->setSegmentStyle(QLCDNumber::Flat);
    simulationTimeLabel->setStyleSheet("background-color: #3A3A60; color: #FFFFFF; border-radius: 6px;");
    simulationTimeLabel->setFixedHeight(35);
    timerLayout->addWidget(simulationTimeLabel);
    elevatorStatusLayout->addLayout(timerLayout);

    ui->fromFloorSpin->setRange(0, numFloors - 1);
    ui->fromFloorSpin->setValue(0);
    ui->directionDown->setDisabled(1);
    ui->elevatorSelectionSpin->setRange(0, numElevators - 1);
    ui->eventLog->appendPlainText("System: Elevator Simulation Started");
}

void MainWindow::incrementSimulationTime() {
    simulationTime++;
    simulationTimeLabel->display(simulationTime);  // Update display with current time
}

void MainWindow::clearLayout(QLayout *layout) {
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) delete widget;
        else if (QLayout *childLayout = item->layout()) clearLayout(childLayout);
        delete item;
    }
}

void MainWindow::upButtonHandler() {
    ecs->getFloorPanel(ui->fromFloorSpin->value())->pressButton(UP);
    ui->eventLog->appendPlainText("UP Button was pressed at " + QString::number(simulationTime) + " seconds.");
}

void MainWindow::downButtonHandler() {
    ecs->getFloorPanel(ui->fromFloorSpin->value())->pressButton(DOWN);
    ui->eventLog->appendPlainText("DOWN Button was pressed at " + QString::number(simulationTime) + " seconds.");
}

void MainWindow::passengerEnterHandler() {
    int elevatorID = ui->elevatorSelectionSpin->value();
    // Only allow passengers to enter if the door is open
    if (ecs->getElevator(elevatorID)->isDoorOpen()) {
        ecs->getElevator(elevatorID)->enterElevator();
    } else {
        ui->eventLog->appendPlainText("Elevator " + QString::number(elevatorID) + ": Door is NOT open!");
    }
}

void MainWindow::passengerExitHandler() {
    int elevatorID = ui->elevatorSelectionSpin->value();
    // Only allow passengers to exit if the door is open
    if (ecs->getElevator(elevatorID)->isDoorOpen()) {
        ecs->getElevator(elevatorID)->exitElevator();
    } else {
        ui->eventLog->appendPlainText("Elevator " + QString::number(elevatorID) + ": Door is NOT open!");
    }
}

void MainWindow::numFloorsSpinHandler() {
    // Update the safe floor spinbox to reflect the new number of floors
    ui->safeFloorSpin->setRange(0, ui->numFloorsSpin->value() - 1);
    if (ui->safeFloorSpin->value() > ui->numFloorsSpin->value() - 1) {
        ui->safeFloorSpin->setValue(ui->numFloorsSpin->value() - 1);
    }
}

void MainWindow::helpButtonHandler() {
    int elevatorID = ui->elevatorSelectionSpin->value();
    // Can't press Help button if elevator is empty (0 passengers inside)
    if (ecs->getElevator(elevatorID)->isEmpty()) {
        ui->eventLog->appendPlainText("Elevator " + QString::number(elevatorID) + ": At least 1 passenger must be inside to press the help button.");
        return;
    }
    ui->eventLog->appendPlainText("HELP Button was pressed at " + QString::number(simulationTime) + " seconds.");
    ecs->getElevator(elevatorID)->handleHelpSignal();
}

void MainWindow::fireAlarmButtonHandler() {
    ui->eventLog->appendPlainText("System: Fire Alarm Detected");
    ui->currentElevatorFrame->setHidden(1);
    ui->controlPanelFrame->setHidden(1);
    ECS::getInstance()->fireAlarm();
}

void MainWindow::localFireAlarmHandler() {
    int elevatorID = ui->elevatorSelectionSpin->value();
    ui->eventLog->appendPlainText("System: Fire Alarm Detected in Elevator " + QString::number(elevatorID));
    ECS::getInstance()->getElevator(elevatorID)->handleFireAlarm();
}

void MainWindow::outageButtonHandler() {
    ui->eventLog->appendPlainText("System: Power Outage Detected");
    ui->currentElevatorFrame->setHidden(1);
    ui->controlPanelFrame->setHidden(1);
    ECS::getInstance()->powerOutage();
}

void MainWindow::fromFloorSpinHandler() {
    if (ui->fromFloorSpin->value() == 0) {
        ui->directionUp->setEnabled(1);
        ui->directionDown->setDisabled(1);
    } 
    
    else if (ui->fromFloorSpin->value() == numFloors - 1) {
        ui->directionUp->setDisabled(1);
        ui->directionDown->setEnabled(1);
    } 
    
    else {
        ui->directionUp->setEnabled(1);
        ui->directionDown->setEnabled(1);
    }
}

void MainWindow::elevatorSelectionHandler() {
    Elevator *elevator = ecs->getElevator(ui->elevatorSelectionSpin->value());
    if (!elevator->isIdle()) {
        addEventLog("Elevator " + std::to_string(ui->elevatorSelectionSpin->value()) + ": Currently MOVING. Please try again later.");
    }
}

void MainWindow::targetFloorButtonHandler() {
    int elevatorID = ui->elevatorSelectionSpin->value();
    if (ecs->getElevator(elevatorID)->isEmpty()) {
        ui->eventLog->appendPlainText("Elevator " + QString::number(elevatorID) + ": At least 1 passenger must be inside to select a floor.");
        return;
    }
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    int floor = button->property("floor").toInt();
    ecs->getElevator(elevatorID)->addSelectedFloor(floor);
}

void MainWindow::updateDisplay(int elevatorID, int floor, int direction) {
    QLCDNumber *lcdNumber = findChild<QLCDNumber *>(QString("elevatorLCD_%1").arg(elevatorID));
    lcdNumber->display(floor);
    QLabel *statusLabel = findChild<QLabel *>(QString("elevatorStatusLabel_%1").arg(elevatorID));
    if (direction == IDLE) {
        statusLabel->setText("IDLE");
    } 
    
    else if (direction == UP) {
        statusLabel->setText("UP");
    } 
    
    else if (direction == DOWN) {
        statusLabel->setText("DOWN");
    }
}