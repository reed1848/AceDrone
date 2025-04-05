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

#include "Grader.h"
#include "Utilities.h"
#include "Parameters.h"
#include "LookupTable.h"

static QUEUING_PORT_ID_TYPE fConfigRequestQueuingPort;
static QUEUING_PORT_ID_TYPE fConfigResponseQueuingPort;

int graderObstacleTimingTable[NUMOBSTACLETYPES][MAXDISTANCE];
Drone drone;
GradeReport lGradeReport = {0};

// Configuration test case
char configInput[MAX_INPUTS][MAXCONFIGPARAMLENGTH];
const char* configTestSet[] = { "Start:Config"
                              , "Color:Blue"
                              , "ID:Columbia"
                              , "Mass:75"
                              , "Cap:5"
                              , "MaxTime:8"
                              , "Fuel:750"
                              , "Vel:15000"
                              , "MPG:5"
                              , "C1:4"
                              , "C2:6"
                              , "Arate:0.5"
                              , "Brate:0.6"
                              , "Erate:0.7"
                              , "Mrate:0.8"
                              , "Srate:0.9"
                              , "End:Config"};

int testCaseLength = sizeof( configTestSet )/sizeof( configTestSet[0] );

/***************************************************************************************************
** grader_main
**  The partition's entry point (called on cold and warm start). Creates processes/ports/etc then
**  transitions to NORMAL mode (NORMAL mode used for Embedded SW and Real-Time Systems Classes)..
**
** Parameters:
**   None
**
** Returns:
**   Nothing
*/
void grader_main( void )
{
    RETURN_CODE_TYPE lReturnCode;
    scoeAmioEnable();

    /* Initialize Ports */
    lReturnCode = initalizePorts();
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to initialize ports: %s", toImage( lReturnCode ) );
    }

    // Copy the configuration test set into config input
    for( int i=0; i<testCaseLength; i++)
    {
        strcpy( configInput[i], configTestSet[i] );
    }

    // Send, receive, and grade student configuration data
    sendConfigData();
    
    // Set parameters and obstacle look up table
    setConfigParameters( &drone, configInput, testCaseLength );
    setLookupTable( &drone, graderObstacleTimingTable );
    
    //Grade configuration response
    receiveConfigData();
    printGradeReport( lGradeReport );

    return;
}

RETURN_CODE_TYPE initalizePorts()
{
    RETURN_CODE_TYPE lReturnCode;

    printf( "--------------- Creating Ports ---------------\n" );
    CREATE_QUEUING_PORT( "ConfigRequestQueuingSender"
                       , 32
                       , 20
                       , SOURCE
                       , PRIORITY
                       , &fConfigRequestQueuingPort
                       , &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
      printf( "Failed to create config request queuing sender port: %s", toImage( lReturnCode ) );
    }
    else
    {
        CREATE_QUEUING_PORT( "ConfigResponseQueuingReceiver"
                           , 64
                           , 20
                           , DESTINATION
                           , PRIORITY
                           , &fConfigResponseQueuingPort
                           , &lReturnCode );
        if ( lReturnCode != NO_ERROR )
        {
          printf( "Failed to create config response queuing receiver port: %s", toImage( lReturnCode ) );
        }
    }

    printf( "--------------- Finished Ports ---------------\n\n" );
    return lReturnCode;
}

void printGradeReport( GradeReport aReport )
{
    int totalCorrect = aReport.configChallenge.correctCount;
    int totalIssued = aReport.configChallenge.totalCount;

    printf( "-------------- Grade Report(s) ---------------\n" );
    printChallengeScore( "Config Challenges", aReport.configChallenge );
    printf( "\nTotal (%d/%d): %f\n", totalCorrect
                                   , totalIssued
                                   , (double)totalCorrect / (double)totalIssued );
}

void printChallengeScore( const char* aTitle, CorrectRatio aRatio )
{
    printf( "%s (%d/%d): %f\n"
          , aTitle
          , aRatio.correctCount
          , aRatio.totalCount
          , (double)aRatio.correctCount / (double)aRatio.totalCount );
}

void updateCRPass( CorrectRatio* aRatio )
{
    aRatio->correctCount++;
    aRatio->totalCount++;
}

void updateCRFail( CorrectRatio* aRatio )
{
    aRatio->totalCount++;
}

// Send config request to student
void sendConfigData()
{
  RETURN_CODE_TYPE lReturnCode;

  printf( "----------- Starting Config Request ----------\n\n" );

  // Loop through config commands
  for( int i = 0; i < testCaseLength; i++ )
  {
      // Send config command
      printf( "Sending: %s\n", configTestSet[i] );
      SEND_QUEUING_MESSAGE( fConfigRequestQueuingPort
                          , (MESSAGE_ADDR_TYPE) configTestSet[i]
                          , strlen( configTestSet[i] ) + 1
                          , 0
                          , &lReturnCode );

      // Verify config command sent succesfully
      if ( lReturnCode != NO_ERROR )
      {
          printf( "Failed to send config request queuing message: %s\n", toImage( lReturnCode ) );
      }
  }

  printf( "\n---------- Config Request Completed ----------\n\n" );
}

// Read configuration response from student
void receiveConfigData()
{
    RETURN_CODE_TYPE lReturnCode;
    char configResponse[MSG_BUFFER_SIZE] = {0};
    MESSAGE_SIZE_TYPE lenMsgData = 0;

    printf( "---------- Awaiting Config Response ----------\n\n" );

    // Loop through each config response
    for( int i=0; i<MAX_OUTPUTS; i++ )
    {
        // Receive config response
        lReturnCode = recvQueuingBlockingMsg( fConfigResponseQueuingPort
                                            , INFINITE_TIME_VALUE
                                            , (MESSAGE_ADDR_TYPE) configResponse
                                            , &lenMsgData );

        if ( lReturnCode != NO_ERROR )
        {
            printf( "Failed to receive config response queuing message: %s\n", toImage( lReturnCode ) );
        }
        else
        {
            // Evaluate student response
            gradeConfigResponse( parameterToString( &drone,
                                                  graderObstacleTimingTable,
                                                  i ), configResponse );
        }
    }

    printf( "---------- Config Response Received ----------\n\n" );
}

//  Determine if config response is correct
void gradeConfigResponse( char *grader, char *student )
{
    printf( "Expected: %s\n", grader );
    printf( "Received: %s\n", student );

    // Verify student's response matches grader's expected value
    if( strcmp( grader, student ) )
    {
        printf( "INCORRECT CONFIG PARAMETER\n\n" );
        updateCRFail( &lGradeReport.configChallenge );
    }
    else
    {
        printf( "CORRECT CONFIG PARAMETER\n\n" );
        updateCRPass( &lGradeReport.configChallenge );
    }
}
