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
void sendFuelData();
void sendStateData();
void receiveIdRequest();
void heartbeat();
// void sendIdResponse();
// void convert_word_to_string(uint16_t word, char* str);

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
