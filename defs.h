#ifndef DEFS_H
#define DEFS_H

#include <iostream>
#include <string>
#include <unistd.h>
#include <queue>

#define IDLE 0
#define UP 1
#define DOWN -1
#define SENSOR_THRESHOLD 10
#define SENSOR_TICK 1000 // 1 second to check sensors
#define HELP_RESPONSE_TIME 5000 // 5 seconds to respond to Help request
#define DEFAULT_COUNTDOWN 10000 // 10 seconds before door closes automatically
#define MOVE_DURATION 5000 // 5 seconds to move one floor
#define DOOROBSTACLE "Doors are obstructed! Please remove obstacle."
#define HELP "Help button was pressed, please wait for assistance."
#define OVERLOAD "Door will remain open until weight is reduced."
#define FIREALARM "FIRE ALARM! Moving to safe floor."
#define POWEROUTAGE "POWER OUTAGE! Moving to safe floor."
#define SAFEFLOOR "Safe floor reached. Evacuate immediately!"

#endif