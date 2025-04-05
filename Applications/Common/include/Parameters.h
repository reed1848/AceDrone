#ifndef _PARAMETERS_H
#define _PARAMETERS_H

/***************************************************************************************************
** Configurable Parameter Variables
*/

typedef enum
{
    AsteroidBelt,
    BlackHole,
    ExplodingSun,
    Mountain,
    ShootingStar,
} Obstacle;

typedef enum
{
    Start,
    ID,
    Mass,
    Cap,
    MaxTime,
    Fuel,
    Vel,
    MPG,
    FuelRate,
    C1,
    C2,
    Arate,
    Brate,
    Erate,
    Mrate,
    Srate,
    End
} ConfigID;

// Configuration Settings
//RATE_OFFSET will always be MAX_INPUTS minus 5
#define MAX_INPUTS 20
#define MAX_OUTPUTS 17
#define RATE_OFFSET 11
#define MAXCONFIGPARAMLENGTH 40

#define NUMOBSTACLETYPES 5
#define MAXDISTANCE 10

#define MAX_OBSTACLE_RATES 5
extern const char *configIds[];

#define MAX_DRONE_NAMES 5
extern const char *droneIds[];

/* Conversion Factors */
/* miles/sec per miles/hour */
#define MPS_PER_MPH 0.000278
#define GALLON_PER_TON 0.0314
#define GALLON_CONSTANT 0.9
#define LBS_PER_PERSON 150
#define TON_PER_LB 0.0005

#define MIN_MASS 70
#define MAX_MASS 90
#define DEFAULT_MASS 80

#define MIN_CAPACITY 1
#define MAX_CAPACITY 5
#define DEFAULT_CAPACITY 3

#define MIN_TIME 8
#define MAX_TIME 10
#define DEFAULT_TIME 10

#define MIN_FUEL 500
#define MAX_FUEL 1000
#define DEFAULT_FUEL 500

#define MIN_VELOCITY 10000
#define MAX_VELOCITY 20000
#define DEFAULT_VELOCITY 20000

#define MIN_MPG 5
#define MAX_MPG 15
#define DEFAULT_MPG 8

#define MIN_C1 1
#define MAX_C1 4
#define DEFAULT_C1 1

#define MIN_C2 2
#define MAX_C2 5
#define DEFAULT_C2 4

#define MIN_RATE 0.5
#define MAX_RATE 1.0
#define DEFAULT_RATE 0.9

typedef struct
{
    char* id;
    int   mass;
    int   capacity;
    int   maxTime;
    float fuel;
    int   velocity;
    int   mpg;
    float fuelRate;
    float   c1;
    float   c2;
    float obstacleRates[MAX_OBSTACLE_RATES];
} Drone;

void setConfigParameters( Drone *drone, char configCommands[MAX_INPUTS][MAXCONFIGPARAMLENGTH], int numOfCommands );
int determineConfigID( char *configId );
void setDroneID( Drone *drone, char *id );
void setDroneMass( Drone *drone, int mass );
void setDroneCapacity( Drone *drone, int cap );
void setDroneMaxTime( Drone *drone, int maxTime );
void setDroneFuel( Drone *drone, float fuel );
void setDroneVelocity( Drone *drone, int velocity );
void setDroneMPG( Drone *drone, int mpg );
void setDroneFuelRate( Drone *drone );
void setDroneC1( Drone *drone, float c1 );
void setDroneC2( Drone *drone, float c2 );
void setDroneObstacleRate( Drone *drone, float rate, int index );
char * parameterToString( Drone *drone, int table[NUMOBSTACLETYPES][MAXDISTANCE], int index );

#endif
