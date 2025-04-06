#ifndef _LOOKUPTABLE_H
#define _LOOKUPTABLE_H

#include "Parameters.h"

#define NUMOBSTACLETYPES 5


void setLookupTable( Drone *drone, int table[NUMOBSTACLETYPES][MAXDISTANCE] );
char * LookupTableToString( int table[NUMOBSTACLETYPES][MAXDISTANCE], const Obstacle obstacle );
int getLookupTableValue( int table[NUMOBSTACLETYPES][MAXDISTANCE], const Obstacle obstacle, const int obstacleDistance );

#endif
