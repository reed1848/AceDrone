
#include <stdbool.h>

#include "../include/DroneStateMachine.h"


DroneState * DroneStateMachine_Init(){
    DroneState *sm = {MiddleCenter};
}

void DroneStateMachine_Avoid_Obstacle(DroneState *sm, Obstacle obstacles[MAX_OBSTACLE_WARNINGS], int num_obstacles){
    enum Position initPos = sm->position;
    enum HorizontalMovement hm;
    enum VerticalMovement vm;
    bool dodgingAsteroid = false;

    //iterate through array and determine vertical and horizontal movement
    // to dodge obstacle
    for(int i=0; i < num_obstacles; i++){
        Obstacle obstacle = obstacles[i];
        switch(obstacle){
            case AsteroidBelt:
                hm = NONE;
                vm = NONE;
                dodgingAsteroid = true;
                break;
            case Mountain:
                vm = (vm == DOWN)? NONE: UP;
                break;
            case ShootingStar:
                vm = (vm == UP)? NONE: DOWN;
                break;
            case BlackHole:
                hm = (hm == LEFT)? NONE: RIGHT;
                break;
            case ExplodingSun:
                hm = (hm == RIGHT)? NONE: LEFT;
                hm = LEFT;
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
    sm->position = move_drone(initPos, hm, vm, dodgingAsteroid);

}

static enum Position move_drone(enum Position initPos, enum HorizontalMovement hm, enum VerticalMovement vm, bool dodgingAsteroid){
    enum Position endPos = initPos;

    // Adjust horizontal boundaries based off of boundaries
    switch(hm){
        case LEFT:
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
            }
            break;
        case RIGHT:
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
            }
            break;
        case NONE:
            //If not dodging horizontall and no asteroid -> move towards center
            if(!dodgingAsteroid){
                if(initPos == BottomRight || BottomLeft){
                    endPos = BottomCenter;
                }
                else if(initPos == MiddleRight || initPos == MiddleLeft){
                    endPos = MiddleCenter;
                }
                else if(initPos == TopRight || initPos == TopLeft){
                    endPos = TopCenter;
                }
            }
            break;
        default:
            break;
    }

    //Adjust vertical movement based on boundaries
    switch(vm){
        case UP:
            switch(initPos){
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
        case DOWN:
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
        case NONE:
            //If not dodging horizontall and no asteroid -> move towards center
            if(!dodgingAsteroid){
                if(initPos == BottomRight || initPos == TopRight){
                    endPos = MiddleRight;
                }
                else if(initPos == BottomCenter || initPos == TopCenter){
                    endPos = MiddleCenter;
                }
                else if(initPos == BottomLeft || initPos == TopLeft){
                    endPos = MiddleLeft;
                }
            }
            break;
        default:
            break;
    }

    return endPos;
}