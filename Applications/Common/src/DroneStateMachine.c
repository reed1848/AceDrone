
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/DroneStateMachine.h"
static const char* PositionStateStrings[] = {
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

static PositionState move_drone(PositionState initPos, HorizontalMovement hm, VerticalMovement vm, bool isDodging);

DroneState * DroneStateMachine_Init(){
    DroneState *sm = (DroneState*)malloc(sizeof(DroneState));
    sm->position = MiddleCenter;
    return sm;
}
 
void DroneStateMachine_Avoid_Obstacle(DroneState *sm, Obstacle obstacles[MAX_OBSTACLE_WARNINGS], int num_obstacles){
    PositionState initPos = sm->position;
    HorizontalMovement hm = NO_HORIZONTAL;
    VerticalMovement vm = NO_VERTICAL;
    bool isDodging = false;
    printf("Start Position Drone: %s\n", PositionStateStrings[initPos]);

    //iterate through array and determine vertical and horizontal movement
    // to dodge obstacle
    for(int i=0; i < num_obstacles; i++){
        Obstacle obstacle = obstacles[i];
        switch(obstacle){
            case AsteroidBelt:
                hm = NO_HORIZONTAL;
                vm = NO_VERTICAL;
                break;
            case Mountain:
                vm = (vm == MOVE_UP)? NO_VERTICAL: MOVE_UP;
                break;
            case ShootingStar:
                vm = (vm == MOVE_UP)? NO_VERTICAL: MOVE_UP;
                break;
            case BlackHole:
                hm = (hm == MOVE_LEFT)? NO_HORIZONTAL: MOVE_RIGHT;
                break;
            case ExplodingSun:
                hm = (hm == MOVE_RIGHT)? NO_HORIZONTAL: MOVE_LEFT;
                break;
            default:
                printf("ERROR: Unrecognized Obstacle");
                break;
        }

        //AsteroidBelt has highest precedence -> Stay still
        if(obstacle == AsteroidBelt){
            break;
        }
    }

    // Move drone within boundaries 
    // Avoids obstacles and moves towards center if possible
    isDodging = num_obstacles > 0;
    printf("HM: %i, VM: %i\n", hm, vm);
    sm->position = move_drone(initPos, hm, vm, isDodging);
    printf("Moved Drone: %s\n", PositionStateStrings[sm->position]);
}

static PositionState move_drone(PositionState initPos, HorizontalMovement hm, VerticalMovement vm, bool isDodging){
    PositionState endPos = initPos;

    // Adjust horizontal boundaries based off of boundaries
    switch(hm){
        case MOVE_LEFT:
            switch(initPos){
                case TopCenter:
                    endPos = TopLeft;
                    break;
                case TopRight:
                    endPos = TopCenter;
                    break;
                case MiddleCenter:
                    endPos = MiddleLeft;
                    break;
                case MiddleRight:
                    endPos = MiddleCenter;
                    break;
                case BottomCenter:
                    endPos = BottomLeft;
                    break;
                case BottomRight:
                    endPos = BottomCenter;
                    break;
                default:
                    break;
            }
            break;
        case MOVE_RIGHT:
            switch(initPos){
                case TopCenter:
                    endPos = TopRight;
                    break;
                case TopLeft:
                    endPos = TopCenter;
                    break;
                case MiddleCenter:
                    endPos = MiddleRight;
                    break;
                case MiddleLeft:
                    endPos = MiddleCenter;
                    break;
                case BottomCenter:
                    endPos = BottomRight;
                    break;
                case BottomLeft:
                    endPos = BottomCenter;
                    break;
                default:
                    break;
            }
            break;
        case NO_HORIZONTAL:
            //If not dodging horizontall and no asteroid -> move towards center
            if(!isDodging){
                if(endPos == BottomRight || endPos == BottomLeft){
                    endPos = BottomCenter;
                }
                else if(endPos == MiddleRight || endPos == MiddleLeft){
                    endPos = MiddleCenter;
                }
                else if(endPos == TopRight || endPos == TopLeft){
                    endPos = TopCenter;
                }
            }
            break;
        default:
            break;
    }

    //Adjust vertical movement based on boundaries
    // NOTE: MUST switch based off endPos so as to not overwrite the horizontal movement
    //       TODO: REFACTOR TO HAVE MORE CLEAR LOGIC
    switch(vm){
        case MOVE_UP:
            switch(endPos){
                case MiddleLeft:
                    endPos = TopLeft;
                    break;
                case MiddleCenter:
                    endPos = TopCenter;
                    break;
                case MiddleRight:
                    endPos = TopRight;
                    break;
                case BottomLeft:
                    endPos = MiddleLeft;
                    break;
                case BottomCenter:
                    endPos = MiddleCenter;
                    break;
                case BottomRight:
                    endPos = MiddleRight;
                    break;
                default:
                    break;
            }
            break;
        case MOVE_DOWN:
            switch(initPos){
                case TopLeft:
                    endPos = MiddleLeft;
                    break;
                case TopCenter:
                    endPos = MiddleCenter;
                    break;
                case TopRight:
                    endPos = MiddleRight;
                    break;
                case MiddleLeft:
                    endPos = BottomLeft;
                    break;
                case MiddleCenter:
                    endPos = BottomCenter;
                    break;
                case MiddleRight:
                    endPos = BottomRight;
                    break;
                default:
                    break;
            }
            break;
        case NO_VERTICAL:
            //If not dodging horizontall and no asteroid -> move towards center
            if(!isDodging){
                if(endPos == BottomRight || endPos == TopRight){
                    endPos = MiddleRight;
                }
                else if(endPos == BottomCenter || endPos == TopCenter){
                    endPos = MiddleCenter;
                }
                else if(endPos == BottomLeft || endPos == TopLeft){
                    endPos = MiddleLeft;
                }
            }
            break;
        default:
            break;
    }

    return endPos;
}