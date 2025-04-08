#ifndef _OBSTACE_HANDLER_H
#define _OBSTACE_HANDLER_H

#include <stdbool.h>


#include "DroneStateMachine.h"
#include "LookupTable.h"

#define MAX_POSITION_STATE_LENGTH 15

#define MAX_OBSTACLES 100

typedef struct {
    Obstacle obstacle;
    int timeToImpact;
} ObstacleTime;


typedef struct {
    ObstacleTime obstacles[MAX_OBSTACLES];
    int count;
} ObstacleHolder;

void ObstacleHolder_Init();
void ObstacleHolder_Update_Obstacle_Times(int deltaTime);
bool ObstacleHolder_Add_Obstacle(Obstacle newObstacle, int time);
void ObstacleHolder_Get_Position_Str();
#endif