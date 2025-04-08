#ifndef _DRONECONTROLLER_H
#define _DRONECONTROLLER_H

#include <apexType.h>

int receiveConfigData();
void sendConfigData();
void initProcesses();
void receiveThread();
void executeThread();
void incrementStateMachineCycle();
void getPositionString();
void addObstaclesToStateMachine();
RETURN_CODE_TYPE initalizePorts();

typedef struct 
{
    union {
        struct {
            int FuelRequest;
            int AstroidDistance;
            int MountainDistance;
            int StarDistance;
            int BlackHoleDistance;
            int ExplodingSunDistance;
        };
        int values[6]; // Array to access all integers at once
    };
    int CycleCounter;
}IncomingUpdate;


#endif
