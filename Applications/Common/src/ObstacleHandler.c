#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ObstacleHandler.h"

const char* PositionStateStrings[] = {
    "TopLeft",
    "TopCenter",
    "TopRight",
    "MiddleLeft",
    "MiddleCenter",
    "MiddleRight",
    "BottomLeft",
    "BottomCenter",
    "BottomRight"
};

const char * ObstacleStrings[] = {
    "AsteroidBelt",
    "BlackHole",
    "ExplodingSun",
    "Mountain",
    "ShootingStar"
};

ObstacleHolder *obstacleHolder;
int DistanceLookUp[NUMOBSTACLETYPES][MAXDISTANCE];
DroneState *droneState;

void ObstacleHolder_Init(){
    obstacleHolder = (ObstacleHolder*)malloc(sizeof(ObstacleHolder));
    obstacleHolder->count = 0;
    //setLookupTable(drone, DistanceLookUp);
    droneState = DroneStateMachine_Init();
}

void ObstacleHolder_Update_Obstacle_Times(int deltaTime){
    Obstacle obstaclesToDodge[MAX_OBSTACLE_WARNINGS];
    int numObstaclesToDodge = 0;

    for (int i = 0; i < obstacleHolder->count; i++) {
        obstacleHolder->obstacles[i].timeToImpact -= deltaTime;
        if (obstacleHolder->obstacles[i].timeToImpact <= 0) {
            // Handle obstacle impact
            obstaclesToDodge[numObstaclesToDodge] = obstacleHolder->obstacles[i].obstacle;
            numObstaclesToDodge++;

            // Remove the obstacle by shifting the remaining obstacles
            for (int j = i; j < obstacleHolder->count - 1; j++) {
                obstacleHolder->obstacles[j] = obstacleHolder->obstacles[j + 1];

            }
            obstacleHolder->count--;
            i--; // Adjust index to account for the removed obstacle
        }
    }


    printf("\n%i obstacles to Dodge:\n", numObstaclesToDodge);
    for(int i=0; i< numObstaclesToDodge; i ++){
        printf("%s\n", ObstacleStrings[ obstaclesToDodge[i]]);
    }
    DroneStateMachine_Avoid_Obstacle(droneState, obstaclesToDodge, numObstaclesToDodge);
    // printf("Position: is %i\n", droneState->position);
    // printf("Position: is %s\n", PositionStateStrings[ droneState->position ]);
}

void ObstacleHolder_Get_Position_Str(char *positionString){
    strcpy(positionString, PositionStateStrings[ droneState->position ]);
}

// Add a new obstacle to the collection
bool ObstacleHolder_Add_Obstacle(Obstacle newObstacle, int time) {
    if (obstacleHolder->count >= MAX_OBSTACLES) {
        // Collection is full
        printf("ERROR: Maximum number of obstacles reached");
        return false;
    }

    obstacleHolder->obstacles[obstacleHolder->count].obstacle = newObstacle;
    obstacleHolder->obstacles[obstacleHolder->count].timeToImpact = time;
    obstacleHolder->count++;
    return true;
}


// // Print the current obstacles
// void printObstacles() {
//     for (int i = 0; i < obstacleHolder->count; i++) {
//         printf("Obstacle %s: Time till impact = %i seconds\n", obstacleHolder->obstacles[i].obstacle, obstacleHolder->obstacles[i].timeToImpact);
//     }
// }

// int main() {
//     printf("Started\n");
//     Drone *drone;
//     ObstacleHolder_Init(drone);
    
//     ObstacleHolder_Add_Obstacle(AsteroidBelt, 2);
//     ObstacleHolder_Add_Obstacle(Mountain, 3);
//     ObstacleHolder_Add_Obstacle(ShootingStar, 4);
//     ObstacleHolder_Add_Obstacle(BlackHole, 5);
//     ObstacleHolder_Add_Obstacle(ExplodingSun, 6);

//     ObstacleHolder_Add_Obstacle(Mountain, 8);
//     ObstacleHolder_Add_Obstacle(ShootingStar, 8);
//     ObstacleHolder_Add_Obstacle(BlackHole, 9);
//     ObstacleHolder_Add_Obstacle(ExplodingSun, 9);

//     ObstacleHolder_Add_Obstacle(Mountain, 10);
//     ObstacleHolder_Add_Obstacle(BlackHole, 10);

//     ObstacleHolder_Add_Obstacle(Mountain, 11);
//     ObstacleHolder_Add_Obstacle(ShootingStar, 11);
//     ObstacleHolder_Add_Obstacle(BlackHole, 11);
//     ObstacleHolder_Add_Obstacle(ExplodingSun, 11);

//     ObstacleHolder_Add_Obstacle(BlackHole, 12);
//     ObstacleHolder_Add_Obstacle(BlackHole, 13);
//     ObstacleHolder_Add_Obstacle(BlackHole, 14);

//     ObstacleHolder_Add_Obstacle(BlackHole, 15);
//     ObstacleHolder_Add_Obstacle(AsteroidBelt, 15);

//     // Simulate clock cycles
//     int deltaTime = 1; // 1 second per cycle
//     for (int cycle = 0; cycle < 20; cycle++) {
//         printf("\nClock cycle %d:\n", cycle + 1);
//         ObstacleHolder_Update_Obstacle_Times( deltaTime);
//     }

//     return 0;
// }
