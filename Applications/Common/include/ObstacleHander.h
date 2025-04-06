#ifndef _OBSTACE_HANDLER_H
#define _OBSTACE_HANDLER_H


#include "DroneStateMachine.h"
#include "LookupTable.h"

#define MAX_OBSTACLES 100

typedef struct {
    Obstacle obstacle;
    int timeToImpact;
} ObstacleTime;


typedef struct {
    ObstacleTime obstacles[MAX_OBSTACLES];
    int count;
} ObstacleHolder;

void ObstacleHolder_Init(Drone *drone);
void ObstacleHolder_Update_Obstacle_Times(int deltaTime);
bool ObstacleHolder_Add_Obstacle(Obstacle newObstacle, int time);

#endif