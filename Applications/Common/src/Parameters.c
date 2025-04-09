#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "../include/Parameters.h"
#include "../include/LookupTable.h"

char *fuelResponseBuffer = NULL;
char *stateResponseBuffer = NULL;

// Config ID parameters
const char * configIds[] = { "Start"
                            ,"ID"
                            ,"Mass"
                            ,"Cap"
                            ,"MaxTime"
                            ,"Fuel"
                            ,"Vel"
                            ,"MPG"
                            ,"FuelRate"
                            ,"C1"
                            ,"C2"
                            ,"Arate"
                            ,"Brate"
                            ,"Erate"
                            ,"Mrate"
                            ,"Srate"
                            ,"End"};

// Valid drone IDs
const char * droneIds[] = { "Columbia"
                           ,"Challenger"
                           ,"Discovery"
                           ,"Atlantis"
                           ,"Endeavour" };

// Valid drone states
const char * droneStates[] = { "TopLeft"
    ,"TopCenter"
    ,"TopRight"
    ,"MiddleLeft"
    ,"MiddleCenter"
    ,"MiddleRight"
    ,"BottomLeft" 
    ,"BottomCenter"
    ,"BottomRight"};

/*
 *  Identifies the provided config ID.
 *  Returns the index of the proper config ID
 *  If no valid config ID is identified, returns -1
*/
int determineConfigID( char *configId )
{

    for( int i=0; i<MAX_OUTPUTS; i++)
    {
        if( strcmp( configId, configIds[i] ) == 0 )
        {
            return i;
        }
    }
    return -1;
}

/*
 *  Sets the drone id parameter.
 *  If a valid id is not provided,
 *  the id is set to Columbia.
*/
void setDroneID( Drone *drone, char *id )
{
    drone->id = "Columbia";

    int i;
    for( i=0; i<MAX_DRONE_NAMES; i++)
    {
        if( strcmp( id, droneIds[i] ) == 0 )
        {
            drone->id = id;
        }
    }
}

/*
 *  Sets the drone mass parameter.
 *  If a valid mass is not provided,
 *  the mass is set to DEFAULT_MASS.
*/
void setDroneMass( Drone *drone, int mass )
{
    drone->mass = DEFAULT_MASS;

    if( ( MIN_MASS <= mass ) && ( mass <= MAX_MASS ) )
    {
        drone->mass = mass;
    }
}

/*
 *  Sets the drone capacity parameter.
 *  If a valid capacity is not provided,
 *  the capacity is set to DEFAULT_CAPACITY.
*/
void setDroneCapacity( Drone *drone, int capacity )
{
    drone->capacity = DEFAULT_CAPACITY;

    if( ( MIN_CAPACITY <= capacity ) && ( capacity <= MAX_CAPACITY ) )
    {
        drone->capacity = capacity;
    }
}

/*
 *  Sets the drone max obstacle time parameter.
 *  If a valid max obstacle time is not provided,
 *  the max obstacle time is set to DEFAULT_TIME.
*/
void setDroneMaxTime( Drone *drone, int maxTime )
{
    drone->maxTime = DEFAULT_TIME;

    if( ( MIN_TIME <= maxTime ) && ( maxTime <= MAX_TIME ) )
    {
        drone->maxTime = maxTime;
    }
}

/*
 *  Sets the drone fuel parameter.
 *  If a valid fuel is not provided,
 *  the fuel is set to DEFAULT_FUEL.
*/
void setDroneFuel( Drone *drone, float fuel )
{
    drone->fuel = DEFAULT_FUEL;

    if( ( MIN_FUEL <= fuel ) && ( fuel <= MAX_FUEL ) )
    {
        drone->fuel = fuel;
    }
}

/*
 *  Sets the drone velocity parameter.
 *  If a valid velocity is not provided,
 *  the velocity is set to DEFAULT_VELOCITY.
*/
void setDroneVelocity( Drone *drone, int velocity )
{
    drone->velocity = DEFAULT_VELOCITY;

    if( ( MIN_VELOCITY <= velocity ) && ( velocity <= MAX_VELOCITY ) )
    {
        drone->velocity = velocity;
    }
}

/*
 *  Sets the drone mpg parameter.
 *  If a valid mpg is not provided,
 *  the mpg is set to DEFAULT_MPG.
*/
void setDroneMPG( Drone *drone, int mpg )
{
    drone->mpg = DEFAULT_MPG;

    if( ( MIN_MPG <= mpg ) && ( mpg <= MAX_MPG ) )
    {
        drone->mpg = mpg;
    }
}

/*
 *  Sets the drone coefficient 1 parameter.
 *  If a valid coefficient 1 is not provided,
 *  the capacity is set to DEFAULT_C1.
*/
void setDroneC1( Drone *drone, float c1 )
{
    drone->c1 = DEFAULT_C1;

    if( ( MIN_C1 <= c1 ) && ( c1 <= MAX_C1 ) )
    {
        drone->c1 = c1;
    }
}

/*
 *  Sets the drone coefficient 2 parameter.
 *  If a valid coefficient 2 is not provided,
 *  the coefficient 2 is set to DEFAULT_C2.
*/
void setDroneC2( Drone *drone, float c2 )
{
    drone->c2 = DEFAULT_C2;

    if( ( MIN_C2 <= c2 ) && ( c2 <= MAX_C2 ) )
    {
        drone->c2 = c2;
    }

    if( drone->c1 > ( drone->c2 - 1 ) )
    {
        drone->c1 = DEFAULT_C1;
    }
}

/*
 *  Sets the drone obstacle rate parameter.
 *  If a valid rate is not provided,
 *  the obstacle rate is set to DEFAULT_RATE.
 *  The index corresponding to the appropriate
 *  obstacle is also provided.
 *  0 - Asteroid
 *  1 - Black Hole
 *  2 - Exploding Sun
 *  3 - Mountain
 *  4 - Shooting Stars
*/
void setDroneObstacleRate( Drone *drone, float rate, int index )
{
    drone->obstacleRates[index - RATE_OFFSET] = DEFAULT_RATE;

    if( ( MIN_RATE <= rate ) && ( rate <= MAX_RATE ) )
    {
        drone->obstacleRates[index - RATE_OFFSET] = rate;
    }
}

/*
 *  Sets the drone fuel rate parameter.
 *  The fuel rate is calculated using the
 *  the previously defined parameters,
 *  conversion factor, and constants.
*/
void setDroneFuelRate( Drone *drone )
{
    drone->fuelRate = roundf( 10 * GALLON_PER_TON * ( drone->mass +
                        ( drone->capacity * LBS_PER_PERSON * TON_PER_LB) )
                         * MPS_PER_MPH * drone->velocity * GALLON_CONSTANT
                          / drone->mpg ) / 10;
}

/*
 *  Sets the drone state parameter.
 *  If a valid state is not provided,
 *  the state is set to TopLeft.
*/
void setDroneState( Drone *drone, char *state )
{
    drone->state = "TopLeft";

    int i;
    for( i=0; i<MAX_DRONE_STATES; i++)
    {
        if( strcmp( state, droneStates[i] ) == 0 )
        {
            drone->state = state;
        }
    }
}

/*
 *  Validates and identifies each config command.
 *  If the config command is valid, the function
 *  sets the appropriate drone parameter.
 *  The expected format: [Valid_Param]:[Value]
*/
void setConfigParameters( Drone *drone, char configCommands[MAX_INPUTS][MAXCONFIGPARAMLENGTH], int numOfCommands )
{
    int i, configIndex;
    const char delim[2] = ":";
    char *configId;
    char *configValue;

    for( i = 0; i < numOfCommands; i++ )
    {
        // Get the configuration parameter id
        configId = strtok( configCommands[i], delim );

        if( configId != NULL )
        {
            // Get the configuration parameter value
            configValue = strtok(NULL, delim);
            if( configValue != NULL)
            {
                // Identify and set the appropriate drone parameter
                configIndex = determineConfigID( configId );
                switch( configIndex )
                {
                    case Start:
                        break;
                    case ID:
                        setDroneID( drone, configValue );
                        break;

                    case Mass:
                        setDroneMass( drone, atoi( configValue ) );
                        break;

                    case Cap:
                        setDroneCapacity( drone, atoi( configValue ) );
                        break;

                    case MaxTime:
                        setDroneMaxTime( drone, atoi( configValue ) );
                        break;

                    case Fuel:
                        setDroneFuel( drone, atof( configValue ) );
                        break;

                    case Vel:
                        setDroneVelocity( drone, atoi( configValue ) );
                        break;

                    case MPG:
                        setDroneMPG( drone, atoi( configValue ) );
                        break;

                    case C1:
                        setDroneC1( drone, atof( configValue ) );
                        break;

                    case C2:
                        setDroneC2( drone, atof( configValue ) );
                        break;

                    case Arate:
                    case Brate:
                    case Erate:
                    case Mrate:
                    case Srate:
                        setDroneObstacleRate( drone, atof( configValue ), configIndex );
                        break;

                    case End:
                        setDroneFuelRate( drone );
                        break;

                    default:
                        printf("Got Unexpected Config ID: %s\n\n", configId );
                }
            }
            else
            {
                printf( "Invalid Config Parameter: %s\n", configCommands[i] );
            }
        }
        else
        {
            printf( "Invalid Config Parameter: %s\n", configCommands[i] );
        }
    }
}

/*
 *  Returns a config string of a drone
 *  parameter given the drone,
 *  the obstacle lookup table, and the
 *  enumerated parameter.
*/
char * parameterToString( Drone *drone, int table[NUMOBSTACLETYPES][MAXDISTANCE], int parameter )
{
    // Allocate adequate space for the config string
    char *message = malloc( MAXCONFIGPARAMLENGTH );

    // Converts the appropriate parameter to a string
    switch( parameter )
    {
        case Start:
            message = "Start - Config Response";
            break;
        case ID:
            sprintf( message, "ID:%s", drone->id );
            break;
        case Mass:
            sprintf( message, "Mass:%d", drone->mass );
            break;
        case Cap:
            sprintf( message, "Cap:%d", drone->capacity );
            break;
        case MaxTime:
            sprintf( message, "MaxTime:%d", drone->maxTime );
            break;
        case Fuel:
            sprintf( message, "Fuel:%.2f", drone->fuel );
            break;
        case Vel:
            sprintf( message, "Vel:%d", drone->velocity );
            break;
        case MPG:
            sprintf( message, "MPG:%d", drone->mpg );
            break;
        case FuelRate:
            sprintf( message, "FuelRate:%.1f", drone->fuelRate);
            break;
        case C1:
            sprintf( message, "C1:%.1f", drone->c1 );
            break;
        case C2:
            sprintf( message, "C2:%.1f", drone->c2 );
            break;
        case Arate:
        case Brate:
        case Erate:
        case Mrate:
        case Srate:
            message = LookupTableToString( table, parameter - RATE_OFFSET );
            break;
        case End:
            message = "End - Config Response";
            break;
        default:
            message = "";
            break;
    }
    return message;
}

char * fuelToString( Drone *drone, int table[NUMOBSTACLETYPES][MAXDISTANCE]){
    sprintf( fuelResponseBuffer, "Fuel: %.1f", drone->fuel );
    return fuelResponseBuffer;
}

char * stateToString( Drone *drone, int table[NUMOBSTACLETYPES][MAXDISTANCE]){
    sprintf( stateResponseBuffer, "%s", drone->state );
    return stateResponseBuffer;
}

void allocateBuffers()
{
    fuelResponseBuffer = (char *)malloc(40);
    if (fuelResponseBuffer == NULL){
        printf("Memory allocation faield for fuelResponseBuffer\n");
    }

    stateResponseBuffer = (char *)malloc(40);
    if (stateResponseBuffer == NULL){
        printf("Memory allocation faield for fuelResponseBuffer\n");
    }
}
