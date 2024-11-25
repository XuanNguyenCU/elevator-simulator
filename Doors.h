#ifndef DOORS_H
#define DOORS_H

class Doors {
private:
    bool openState;

public:
    Doors() : openState(false) {} // Initialize door as closed

    void open() {
        if (!openState) openState = true;
    }

    void close() {
        if (openState) openState = false;
    }

    bool isOpened() const { return openState; } // Check if door is open
};

#endif // DOORS_H
