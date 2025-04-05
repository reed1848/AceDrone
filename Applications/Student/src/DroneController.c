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

static QUEUING_PORT_ID_TYPE fConfigRequestQueuingPort;
static QUEUING_PORT_ID_TYPE fConfigResponseQueuingPort;

int numOfConfigCommands = 0;
char configCommands[MAX_INPUTS][MAXCONFIGPARAMLENGTH];
int obstacleTimingLookupTable[NUMOBSTACLETYPES][MAXDISTANCE];
Drone student;

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
    }

    // Receive grader's config request
    commandCount = receiveConfigData();
    
    // Set configuration parameters and obstacle lookup table
    setConfigParameters( &student, configCommands, commandCount );
    setLookupTable( &student, obstacleTimingLookupTable );

    // Send student config response
    sendConfigData();

    return;
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
            printf( "Failed to receive config request message: %s\n", toImage( lReturnCode ) );
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

    printf( "--------------- Finished Ports ---------------\n\n" );
    return lReturnCode;
}
