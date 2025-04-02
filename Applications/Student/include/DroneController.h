#ifndef _DRONECONTROLLER_H
#define _DRONECONTROLLER_H

#include <unordered_map>
// Global vars
extern std::unordered_map<std::string, std::string> msgValues;
#ifdef __cplusplus
extern "C" {
#endif

void droneController_main( void );
#ifdef __cplusplus
}
#endif

#endif