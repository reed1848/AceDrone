#ifndef _DRONECONTROLLER_H
#define _DRONECONTROLLER_H

#include <apexType.h>

int receiveConfigData();
void sendConfigData();
void initProcesses();
void receiveThread();
RETURN_CODE_TYPE initalizePorts();

#endif
