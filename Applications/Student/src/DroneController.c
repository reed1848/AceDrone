/***************************************************************************************************
** Copyright 2014 GE Aviation Systems LLC, U.S.A.
** -------------------------------------------------------------------------------------------------
**                       GE AVIATION SYSTEMS LLC PROPRIETARY INFORMATION
** The information contained in this document is GE Aviation Systems LLC proprietary information
** and is disclosed in confidence.  It is the property of GE Aviation Systems LLC and shall not
** be used, disclosed to others or reproduced without the express written consent of GE Aviation
** Systems LLC, including, but without limitation, it is not to be used in the creation,
** manufacture, development, or derivation of any repairs, modifications, spare parts, designs, or
** configuration changes or to obtain FAA or any other government or regulatory approval to do so.
** If consent is given for reproduction in whole or in part, this notice and the notice set forth
** on each page of this document shall appear in any such reproduction in whole or in part.
**
** This technical data is considered ITAR and/or EAR controlled pursuant to 22 CFR Part 120-130
** and 15 CFR Parts 730-774 respectively.  Transfer of this data by any means to a Non-US
** person, whether in the United States or abroad, without the proper U.S. Government
** authorization (e.g.,License, exemption, NLR, etc.) is strictly prohibited.
***************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <apexType.h>
#include <apexProcess.h>
#include <apexPartition.h>
#include <apexQueuing.h>
#include <apexSampling.h>
#include "apex/apexLib.h"

#include <scoeAMIOEnable.h>

#include "DroneController.h"
#include "Utilities.h"
#include "Parameters.h"
#include "LookupTable.h"
#include "../../Common/include/ObstacleHandler.h"
#include "../../Common/include/DroneStateMachine.h"

#define RECEIVE_PROCESS_PERIOD 500000000LL  // every 0.5 seconds
#define ID_PROCESS_PERIOD 3000000000LL  //every 3 seconds
#define HEARTBEAT_PROCESS_PERIOD 4000000000LL  //every 3 seconds

#define MAX_PROCESS_TIME 250000000LL
#define DEFAULT_PRIORITY 1

#define INFINITE_TIME -1
//#define RECEIVE_PROCESS_CAPACITY -1LL
#define PROCESS_STACK_SIZE 8000
#define NA -1
#define MAX_UPDATE_STRING_SIZE 35
#define SEMAPHORE_MAX 1
#define SEMAPHORE_INIT 1

#define ONE_CYCLE 1

#define ID_REQUEST_MESSAGE 0xCABE
#define HEARTBEAT_MESSAGE 0xF5A6
#define MAX_ID_LENGTH 20

static QUEUING_PORT_ID_TYPE fConfigRequestQueuingPort;
static QUEUING_PORT_ID_TYPE fConfigResponseQueuingPort;
static QUEUING_PORT_ID_TYPE fResponseQueuingSender;
static QUEUING_PORT_ID_TYPE fCommandQueuingReceiver;
static QUEUING_PORT_ID_TYPE fIdRequestQueuingPort;
static QUEUING_PORT_ID_TYPE fIdResponseQueuingPort;
static QUEUING_PORT_ID_TYPE fHeartBeatResponseQueuingPort;

int numOfConfigCommands = 0;
char configCommands[MAX_INPUTS][MAXCONFIGPARAMLENGTH];
int obstacleTimingLookupTable[NUMOBSTACLETYPES][MAXDISTANCE];

PROCESS_ID_TYPE receiveThreadID;
PROCESS_ID_TYPE executeThreadID;
PROCESS_ID_TYPE idThreadID;
PROCESS_ID_TYPE heartbeatThreadID;

EVENT_ID_TYPE execEventID;
EVENT_ID_TYPE sendFuelResponseID;
bool fuelResponseRequested;
SEMAPHORE_ID_TYPE configDataSemaphore;
Drone student;
IncomingUpdate updateData;
BOOL shutdownFlag = FALSE;
clock_t startTime;
double cycles = 0;

char idResponse[MAX_ID_LENGTH];
char heartbeatMessage[5];
char sendStateFlag = true;

//char *fuelResponseBuffer = NULL;

char positionString[MAX_POSITION_STATE_LENGTH];

/***************************************************************************************************
** droneController_main
**  The partition's entry point (called on cold and warm start). Creates processes/ports/etc then
**  transitions to NORMAL mode (NORMAL mode used for Embedded SW and Real-Time Systems Classes).
**
** Parameters:
**   None
**
** Returns:
**   Nothing
*/
void droneController_main( void )
{
    RETURN_CODE_TYPE lReturnCode;
    int commandCount;

    scoeAmioEnable();

    /* Initialize Ports */
    lReturnCode = initalizePorts();
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to initialize ports: %s", toImage( lReturnCode ) );
        return;
    }

    // Receive grader's config request
    commandCount = receiveConfigData();

    // Set configuration parameters and obstacle lookup table
    setConfigParameters( &student, configCommands, commandCount );
    setLookupTable( &student, obstacleTimingLookupTable );

    // Send student config response
    sendConfigData();

    // Create receiving thread to receive and process incoming messages from grader
    initProcesses();

    /**************************************************/
    /* enter NORMAL partition mode to begin operation */
    /**************************************************/
    printf( "Entering normal mode ... \n" );
    SET_PARTITION_MODE ( NORMAL, &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to enter normal mode %s", toImage( lReturnCode ) );
    }
    startTime = clock();


    return;
}

void initProcesses()
{

    //init statemachine
    ObstacleHolder_Init();
    allocateBuffers();
    setDroneState(&student, "BottomCenter");

    RETURN_CODE_TYPE lReturnCode;

    /*
     * Create receive thread
    */
    PROCESS_ATTRIBUTE_TYPE receiveThreadAttr =
    {
        "RECEIVE_THREAD",               /* Name          */
        receiveThread,                  /* Entry point   */
        PROCESS_STACK_SIZE,             /* Stack size    */
        DEFAULT_PRIORITY,                             /* Priority      */
        RECEIVE_PROCESS_PERIOD,         /* Period        */
        INFINITE_TIME,                  /* Time capacity */
        SOFT                            /* Deadline type */
    };

    printf("Creating Receive Thread ...\n");
    CREATE_PROCESS (
    &receiveThreadAttr,   /* process attribute */
    &receiveThreadID, /* process Id        */
    &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create process: %s \n", toImage( lReturnCode ) );
        return;
    }

    printf("Starting Receive thread \n");
    START(receiveThreadID, &lReturnCode);
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to start process: %s \n", toImage( lReturnCode ) );
        return;
    }

    /*
     * Set up execute thread
    */
     PROCESS_ATTRIBUTE_TYPE execThreadAttr =
    {
        "EXEC_THREAD",                  /* Name          */
        executeThread,                  /* Entry point   */
        PROCESS_STACK_SIZE,             /* Stack size    */
        DEFAULT_PRIORITY,                              /* Priority      */
        INFINITE_TIME,                  /* Period        */
        INFINITE_TIME,                  /* Time capacity */
        SOFT                            /* Deadline type */
    };

    printf("Creating Execute Thread ...\n");
    CREATE_PROCESS (
    &execThreadAttr,   /* process attribute */
    &executeThreadID, /* process Id        */
    &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create process: %s \n", toImage( lReturnCode ) );
        return;
    }

    printf("Starting Execute Thread \n");
    START(executeThreadID, &lReturnCode);
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to start process: %s \n", toImage( lReturnCode ) );
        return;
    }


    /*
     *setup id request thread
    */
    PROCESS_ATTRIBUTE_TYPE idThreadAttr =
    {
        "ID_THREAD",                  /* Name          */
        receiveIdRequest,                  /* Entry point   */
        PROCESS_STACK_SIZE,             /* Stack size    */
        DEFAULT_PRIORITY,                              /* Priority      */
        ID_PROCESS_PERIOD,                  /* Period        */
        MAX_PROCESS_TIME,                  /* Time capacity */
        SOFT                            /* Deadline type */
    };

    printf("Creating ID Thread ...\n");
    CREATE_PROCESS (
    &idThreadAttr,   /* process attribute */
    &idThreadID, /* process Id        */
    &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create process: %s \n", toImage( lReturnCode ) );
        return;
    }
    printf("Starting ID Thread \n");
    START(idThreadID, &lReturnCode);
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to start process: %s \n", toImage( lReturnCode ) );
        return;
    }

    /*
     * setup heartbeat message thread
     */
    PROCESS_ATTRIBUTE_TYPE heartbeatThreadAttr =
    {
        "HEARTBEAT_THREAD",                  /* Name          */
        heartbeat,                  /* Entry point   */
        PROCESS_STACK_SIZE,             /* Stack size    */
        DEFAULT_PRIORITY,                              /* Priority      */
        HEARTBEAT_PROCESS_PERIOD,                  /* Period        */
        MAX_PROCESS_TIME,                  /* Time capacity */
        SOFT                            /* Deadline type */
    };

    printf("Creating Heartbeat Thread ...\n");
    CREATE_PROCESS (
    &heartbeatThreadAttr,   /* process attribute */
    &heartbeatThreadID, /* process Id        */
    &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create process: %s \n", toImage( lReturnCode ) );
        return;
    }
    printf("Starting Heartbeat Thread \n");
    START(heartbeatThreadID, &lReturnCode);
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to start process: %s \n", toImage( lReturnCode ) );
        return;
    }


    // Create an event to wake up execute thread
    EVENT_NAME_TYPE eventName = "ExecuteEvent";
    CREATE_EVENT( eventName, &execEventID, &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create event: %s \n", toImage( lReturnCode ) );
        return;
    }

    // CREATE a semaphore to access shared resource
    CREATE_SEMAPHORE("IncomingUpdateSemaphore", SEMAPHORE_INIT, SEMAPHORE_MAX, FIFO, &configDataSemaphore, &lReturnCode);
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to semaphore: %s \n", toImage( lReturnCode ) );
        return;
    }
}

int receiveConfigData()
{
    MESSAGE_SIZE_TYPE lenMsgData = 0;
    RETURN_CODE_TYPE lReturnCode;
    int messageCount = 0;

    printf( "----------- Recieving Config Request ----------\n\n" );
    FOREVER
    {
        // Read the message
        lReturnCode = recvQueuingMsg( fConfigRequestQueuingPort
                                    , (MESSAGE_ADDR_TYPE) configCommands[messageCount]
                                    , &lenMsgData );

        // Verify grader config request received correctly
        if ( lReturnCode != NO_ERROR )
        {
            //printf( "Failed to receive config request message: %s\n", toImage( lReturnCode ) );
        }
        else
        {
            printf( "Received: %s\n", configCommands[messageCount] );
            messageCount++;
            if( strcmp( configCommands[messageCount-1], "End:Config" ) == 0 )
            {
                printf( "----------- Recieving Config Request ----------\n\n" );
                return messageCount;
            }
        }
    }
}

void executeThread()
{
    RETURN_CODE_TYPE lReturnCode;
    while(shutdownFlag != TRUE)
    {
        WAIT_EVENT( execEventID, INFINITE_TIME, &lReturnCode );
        if ( lReturnCode != NO_ERROR )
        {
            printf( "Failed to wait for event: %s \n", toImage( lReturnCode ) );
        }
        WAIT_SEMAPHORE(configDataSemaphore, INFINITE_TIME, &lReturnCode);
        if ( lReturnCode != NO_ERROR )
        {
            printf( "Failed to wait for semaphore: %s \n", toImage( lReturnCode ) );
        }

        // Run code here
        //printf("Running execute code here: \n");

        addObstaclesToStateMachine();
        incrementStateMachineCycle();
        getPositionString();
        int length = strlen(positionString);

        /////
        /////
        //sendStateData();

        if (updateData.FuelRequest == TRUE)
        {

            setDroneFuel(&student, floorf((1000.0f - ((float)updateData.CycleCounter * student.fuelRate)) * 10.0f) / 10.0f);
            sendFuelData();
        }
        if(sendStateFlag){
            SEND_QUEUING_MESSAGE(
                fResponseQueuingSender,
                (MESSAGE_ADDR_TYPE) positionString,
                length + 1,
                0,
                &lReturnCode );
        }
        sendStateFlag = !sendStateFlag;
        SIGNAL_SEMAPHORE(configDataSemaphore, &lReturnCode);
        if ( lReturnCode != NO_ERROR )
        {
            printf( "Failed to set semaphore: %s \n", toImage( lReturnCode ) );
        }
        RESET_EVENT( execEventID, &lReturnCode );
        if ( lReturnCode != NO_ERROR )
        {
            printf( "Failed to reset event: %s \n", toImage( lReturnCode ) );
        }
    }
}

void heartbeat(){
    RETURN_CODE_TYPE retCode;

    int loopCounter = 1;
    while(shutdownFlag != TRUE){
        printf("Heartbeat Counter: %i\n", loopCounter++);
        if(retCode == NO_ERROR){
            // Log ID Request
            //Note: since port acts as a queue, must sent bytes in reverse order
            heartbeatMessage[1] = (char)(HEARTBEAT_MESSAGE & 0xFF);       // Lower byte
            heartbeatMessage[0] = (char)((HEARTBEAT_MESSAGE >> 8) & 0xFF); // Upper byte
            heartbeatMessage[2] = '\0';
            printf("Heartbeat Message: %s\n", heartbeatMessage);

            RETURN_CODE_TYPE retCode;
            //Send config response message
            SEND_QUEUING_MESSAGE(
                fHeartBeatResponseQueuingPort,
                (MESSAGE_ADDR_TYPE) heartbeatMessage,
                strlen(heartbeatMessage) + 1,
                0,
                &retCode );
        
            // Verify config response sent sucessfully
            if ( retCode != NO_ERROR )
            {
                printf( "Failed to send Heartbeat message: %s\n", toImage( retCode ) );
            }
            else
            {
                printf( "Sent Heartbeat Message: %s\n", heartbeatMessage );
            }
        

            printf("Sent HeartBeat Response\n");

        }
        PERIODIC_WAIT(&retCode);
    }
}

void receiveIdRequest(){
    RETURN_CODE_TYPE retCode;
    MESSAGE_SIZE_TYPE lenMsgData = 0;
    unsigned char incomingIdRequest[MAX_UPDATE_STRING_SIZE] = {};

    int loopCounter = 1;
    while(shutdownFlag != TRUE){
        printf("Receive ID Loop %i\n", loopCounter++);
        retCode = recvQueuingMsg(fIdRequestQueuingPort, incomingIdRequest, &lenMsgData);
        if(retCode == NO_ERROR && lenMsgData > 0){
            // Log ID Request
            printf("\nReceived ID Request\n");
            sprintf(idResponse, "%s", student.id);                // Null-terminate the string
            printf("Converted ID: %s\n", idResponse);

            RETURN_CODE_TYPE retCode;
            //Send config response message
            SEND_QUEUING_MESSAGE(
                fIdResponseQueuingPort,
                (MESSAGE_ADDR_TYPE) idResponse,
                strlen(idResponse) + 1,
                0,
                &retCode );
        
            // Verify config response sent sucessfully
            if ( retCode != NO_ERROR )
            {
                printf( "Failed to send ID response message: %s\n", toImage( retCode ) );
            }
            else
            {
                printf( "Sent ID Response: %s\n", idResponse );
            }
        

            printf("Sent ID Response\n");

        }
        PERIODIC_WAIT(&retCode);
    }
}

// void sendIdResponse(){
//     RETURN_CODE_TYPE retCode;
//     //convert_word_to_string(ID_RESPONSE_MESSAGE, idResponse);
//     //Send config response message
//     SEND_QUEUING_MESSAGE(
//         fIdResponseQueuingPort,
//         (MESSAGE_ADDR_TYPE) idResponse,
//         strlen(idResponse) + 1,
//         0,
//         &retCode );

//     // Verify config response sent sucessfully
//     if ( retCode != NO_ERROR )
//     {
//         printf( "Failed to send ID response message: %s\n", toImage( retCode ) );
//     }
//     else
//     {
//         printf( "Sending ID Response: %s\n", idResponse );
//     }
// }

// void convert_word_to_string(uint16_t word, char* str) {
    
// }

void receiveThread()
{
    updateData.CycleCounter = 0;
    RETURN_CODE_TYPE retCode;
    MESSAGE_SIZE_TYPE lenMsgData = 0;
    unsigned char incomingObstacleUpdate[MAX_UPDATE_STRING_SIZE] = {};

    while (shutdownFlag != TRUE)
    {
        int messageCounter = 0;
        /* Read in the string from the receivePort, nonblocking call */
        retCode = recvQueuingMsg(fCommandQueuingReceiver, incomingObstacleUpdate, &lenMsgData);
        if (retCode == NO_ERROR)
        {
            //printf("in decoding string %d with value %s \n", updateData.CycleCounter, incomingObstacleUpdate);
            WAIT_SEMAPHORE(configDataSemaphore, INFINITE_TIME, &retCode);
            if ( retCode != NO_ERROR )
            {
                printf( "Failed to wait for semaphore: %s \n", toImage( retCode ) );
            }
            /* Reset update data upon successful read */
            memset(updateData.values, NA, sizeof(updateData.values));
            /* Parse string and update struct*/
            while (messageCounter < lenMsgData)
            {
                if ((messageCounter == 0) && incomingObstacleUpdate[0] == 'F')
                {
                    updateData.FuelRequest = TRUE;
                }
                else
                {
                    /* Parse the first letter and grab the associated distance */
                    //printf("Message Output:\n");
                    //printf("%c\n", incomingObstacleUpdate[messageCounter]);
                    //printf("%c\n", incomingObstacleUpdate[messageCounter +1]);

                    switch (incomingObstacleUpdate[messageCounter])
                    {
                        case 'A':
                            //printf("Asteroid Test: %i\n", (int) incomingObstacleUpdate[messageCounter + 1]);
                            updateData.AstroidDistance = (int) incomingObstacleUpdate[++messageCounter] - '0';
                            break;
                        case 'M':
                            updateData.MountainDistance = (int) incomingObstacleUpdate[++messageCounter] - '0';
                            break;
                        case 'S':
                            updateData.StarDistance = (int) incomingObstacleUpdate[++messageCounter] - '0';
                            break;
                        case 'B':
                            updateData.BlackHoleDistance = (int) incomingObstacleUpdate[++messageCounter] - '0';
                            break;
                        case 'E':
                            updateData.ExplodingSunDistance = (int)incomingObstacleUpdate[++messageCounter] - '0';
                            break;
                        default:
                            break;
                    }
                }
                messageCounter++;
            }
            updateData.CycleCounter += 1;
            SIGNAL_SEMAPHORE(configDataSemaphore, &retCode);
            if ( retCode != NO_ERROR )
            {
                printf( "Failed to set semaphore: %s \n", toImage( retCode ) );
            }

            SET_EVENT( execEventID, &retCode );
            if ( retCode != NO_ERROR )
            {
                printf( "Failed to set event: %s \n", toImage( retCode ) );
            }



        }

        /************************************/
        /* Wait till next period expiration */
        /************************************/
        PERIODIC_WAIT ( &retCode );
    }
    printf("Shutdown flag activated?? ...\n");
    STOP_SELF();
}

void sendConfigData()
{
    RETURN_CODE_TYPE lReturnCode;
    char *message;

    printf( "---------- Starting Config Response ----------\n\n" );

    // Loop through config response commands
    for( int i=0; i<MAX_OUTPUTS; i++ )
    {
        // set message for config response
        message = parameterToString( &student, obstacleTimingLookupTable, i );

        //Send config response message
        SEND_QUEUING_MESSAGE(
            fConfigResponseQueuingPort,
            (MESSAGE_ADDR_TYPE) message,
            strlen( message ) + 1,
            0,
            &lReturnCode );

        // Verify config response sent sucessfully
        if ( lReturnCode != NO_ERROR )
        {
            printf( "Failed to send config response message: %s\n", toImage( lReturnCode ) );
        }
        else
        {
            printf( "Sending: %s\n", message );
        }
    }

    printf( "\n---------- Config Response Complete ----------\n\n" );
}

void sendFuelData()
{
    RETURN_CODE_TYPE lReturnCode;
    char *message;

    printf( "---------- Starting Fuel Response ----------\n\n" );
    // set message for fuel response
    message = fuelToString(&student, obstacleTimingLookupTable);

    //Send fuel response message
    SEND_QUEUING_MESSAGE(
        fResponseQueuingSender,
        (MESSAGE_ADDR_TYPE) message,
        strlen( message ) + 1,
        0,
        &lReturnCode );

    // Verify config response sent sucessfully
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to send fuel response message: %s\n", toImage( lReturnCode ) );
    }
    else
    {
        printf( "Sending: %s\n", message );
    }

    printf( "\n---------- Fuel Response Complete ----------\n\n" );
}

void sendStateData()
{
    RETURN_CODE_TYPE lReturnCode;
    char *message;

    printf( "---------- Starting State Response ----------\n\n" );
    // set message for State response
    message = stateToString(&student, obstacleTimingLookupTable);

    //Send fuel response message
    SEND_QUEUING_MESSAGE(
        fResponseQueuingSender,
        (MESSAGE_ADDR_TYPE) message,
        strlen( message ) + 1,
        0,
        &lReturnCode );

    // Verify config response sent sucessfully
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to send state response message: %s\n", toImage( lReturnCode ) );
    }
    else
    {
        printf( "Sending: %s\n", message );
    }

    printf( "\n---------- State Response Complete ----------\n\n" );
}


RETURN_CODE_TYPE initalizePorts()
{
    RETURN_CODE_TYPE lReturnCode;

    printf( "--------------- Creating Ports ---------------\n" );

    CREATE_QUEUING_PORT( "ConfigRequestQueuingReceiver"
						 , 64
						 , 20
						 , DESTINATION
						 , PRIORITY
						 , &fConfigRequestQueuingPort
						 , &lReturnCode );

    if ( lReturnCode != NO_ERROR )
    {
		    printf( "Failed to create config request queuing receiver port: %s", toImage( lReturnCode ) );
    }
    else
    {
        CREATE_QUEUING_PORT( "ConfigResponseQueuingSender"
							 , 32
							 , 20
							 , SOURCE
							 , PRIORITY
							 , &fConfigResponseQueuingPort
							 , &lReturnCode );
        if ( lReturnCode != NO_ERROR )
        {
			       printf( "Failed to create config response queuing sender port: %s", toImage( lReturnCode ) );
        }
    }


    CREATE_QUEUING_PORT( "CommandQueuingReceiver"
        , 64
        , 4
        , DESTINATION
        , PRIORITY
        , &fCommandQueuingReceiver
        , &lReturnCode );

    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create CommandQueuingReceiver port: %s", toImage( lReturnCode ) );
    }
    else
    {
        CREATE_QUEUING_PORT( "ResponseQueuingSender"
                    , 32
                    , 4
                    , SOURCE
                    , PRIORITY
                    , &fResponseQueuingSender
                    , &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create ResponseQueuingSender port: %s", toImage( lReturnCode ) );
    }
    }


    CREATE_QUEUING_PORT( "IDRequestQueuingReceiver"
        , 64
        , 4
        , DESTINATION
        , PRIORITY
        , &fIdRequestQueuingPort
        , &lReturnCode );

    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create IDRequestQueuingReceiver port: %s", toImage( lReturnCode ) );
    }
    else
    {
        CREATE_QUEUING_PORT( "IDResponseQueuingSender"
                    , 32
                    , 4
                    , SOURCE
                    , PRIORITY
                    , &fIdResponseQueuingPort
                    , &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create IDResponseQueuingSender port: %s", toImage( lReturnCode ) );
    }
    }

    //Create HeartBeat Port
    CREATE_QUEUING_PORT( "HeartbeatQueuingSender"
                , 32
                , 4
                , SOURCE
                , PRIORITY
                , &fHeartBeatResponseQueuingPort
                , &lReturnCode );


    printf( "--------------- Finished Ports ---------------\n\n" );
    return lReturnCode;
}


void addObstaclesToStateMachine(){

    printf("........................................................");
    //printf("\nPrinting contents of update data:\n");
    for(int i=1; i < 6; i++){
        //printf("%i:%i\n", i, updateData.values[i]);
    }
    //printf("\n");

    if(updateData.AstroidDistance >= 0){
        int distance = getLookupTableValue(obstacleTimingLookupTable, AsteroidBelt, updateData.AstroidDistance);
        ObstacleHolder_Add_Obstacle(AsteroidBelt, distance);
    }
    if(updateData.BlackHoleDistance >= 0){
        int distance = getLookupTableValue(obstacleTimingLookupTable, BlackHole, updateData.BlackHoleDistance);
        ObstacleHolder_Add_Obstacle(BlackHole, distance);
    }
    if(updateData.ExplodingSunDistance >= 0){
        int distance = getLookupTableValue(obstacleTimingLookupTable, ExplodingSun, updateData.ExplodingSunDistance);
        ObstacleHolder_Add_Obstacle(ExplodingSun, distance);
    }
    if(updateData.MountainDistance >=0){
        int distance = getLookupTableValue(obstacleTimingLookupTable, Mountain, updateData.MountainDistance);
        ObstacleHolder_Add_Obstacle(Mountain, distance);
    }
    if(updateData.StarDistance >= 0){
        int distance = getLookupTableValue(obstacleTimingLookupTable, ShootingStar, updateData.StarDistance);
        ObstacleHolder_Add_Obstacle(ShootingStar, distance);
    }

}

void incrementStateMachineCycle(){
    ObstacleHolder_Update_Obstacle_Times(ONE_CYCLE);
}

void getPositionString(){
    ObstacleHolder_Get_Position_Str(positionString);
}
