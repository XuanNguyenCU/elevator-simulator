#ifndef FLOOR_H
#define FLOOR_H

#include "defs.h"
#include "mainwindow.h"

class Floor {

public:
    explicit Floor(int floorNum); // ECS will be initialized
    void pressButton(int direction);
    void reset();  // Reset the floor button states

private:
    int floorNum;
    bool upButtonIlluminated;
    bool downButtonIlluminated;

};

#endif
