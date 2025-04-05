#include "Parameters.h"

#define MAX_OBSTACLE_WARNINGS 5


enum Position {
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    MiddleCenter,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

enum HorizontalMovement{
    LEFT,
    RIGHT,
    NONE
};

enum VerticalMovement{
    UP,
    DOWN,
    NONE
};

typedef struct {
    enum Position position;
} DroneState;

typedef struct  {
    int fuelRequestFlag;
    ObstacleDistance obstacleDistances[MAX_OBSTACLE_WARNINGS];
    int counter;
} request;

typedef struct {
    enum Obstacle obstacle;
    int distance;
} ObstacleDistance;