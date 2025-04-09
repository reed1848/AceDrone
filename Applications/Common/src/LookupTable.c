#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Parameters.h"
#include "../include/LookupTable.h"

// Populates lookup table with obstacle times per drone configuration
void setLookupTable( Drone *drone, int table[NUMOBSTACLETYPES][MAXDISTANCE] )
{
    float constant, configFactor, timingFactor;
    Obstacle loopObstacle;
    int loopDistance, obstacleTime;

    configFactor = drone->c1 / drone->c2;

    // Loop through each obstacle in table
    for( loopObstacle = AsteroidBelt; loopObstacle <= ShootingStar; loopObstacle++ )
    {
        // Calculate timingFactor for obstacle
        constant = drone->obstacleRates[loopObstacle];
        timingFactor = constant * configFactor;

        // Populate obstacle time for each obstacle distance
        for( loopDistance = 0; loopDistance < MAXDISTANCE; loopDistance++ )
        {
            obstacleTime = (int) ceilf( fmod( ( loopDistance * timingFactor ), drone->maxTime ) );
            table[loopObstacle][loopDistance] = obstacleTime;
        }

    }
}

// Outputs a string of obstacle times from lookup table
char * LookupTableToString( int table[NUMOBSTACLETYPES][MAXDISTANCE], const Obstacle obstacle )
{
    int loopDistance = 0;

    // Allocate adequate memory space for output and temporary pointer
    char *output = malloc( ( MAXDISTANCE - 1 ) * sizeof( int ) + strlen(configIds[obstacle]) );
    char *tmp = malloc( strlen("s:") + sizeof( int ) );

    // Copy config ID and first obstacle time into output
    strcpy( output, configIds[obstacle + RATE_OFFSET] );
    sprintf( tmp, "s:%d", table[obstacle][loopDistance] );
    strcat( output, tmp );

    // Copy remaining obstacle times into output
    for( loopDistance = 1; loopDistance < MAXDISTANCE; loopDistance++ )
    {
        sprintf( tmp, ",%d", table[obstacle][loopDistance] );
        strcat( output, tmp );
    }
    return output;
}

// Outputs the obstacle time for a given lookup table, obstacle, and distance
int getLookupTableValue( int table[NUMOBSTACLETYPES][MAXDISTANCE], const Obstacle obstacle, const int obstacleDistance )
{
    return table[obstacle][obstacleDistance];
}
