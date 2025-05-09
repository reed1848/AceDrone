#ifndef _DRONE_STATE_MACHINE_H
#define _DRONE_STATE_MACHINE_H

#include "Parameters.h"

#define MAX_OBSTACLE_WARNINGS 5


typedef enum  {
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    MiddleCenter,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight
}PositionState;

//NOTE: enum of basic directions already taken by apex library
// Hence weird direction names
typedef enum {
    MOVE_LEFT,
    MOVE_RIGHT,
    NO_HORIZONTAL
}HorizontalMovement;

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    NO_VERTICAL
}VerticalMovement;

typedef struct {
    PositionState position;
} DroneState;


typedef struct {
    Obstacle obstacle;
    int distance;
} ObstacleDistance;

typedef struct  {
    int fuelRequestFlag;
    ObstacleDistance obstacleDistances[MAX_OBSTACLE_WARNINGS];
    int counter;
} request;

DroneState * DroneStateMachine_Init();
void DroneStateMachine_Avoid_Obstacle(DroneState *sm, Obstacle obstacles[MAX_OBSTACLE_WARNINGS], int num_obstacles);

#endif