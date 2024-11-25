#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

class Elevator;

class ControlPanel {

public:
    explicit ControlPanel(Elevator* elevator);
    void chooseFloorButton(int floorNum) const;  // Selects a floor for the elevator to go to
    void pressOpenDoor() const;
    void pressCloseDoor() const;
    void pressHelpButton() const;

private:
    Elevator* elevator;

};

#endif
