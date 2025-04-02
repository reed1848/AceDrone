/***************************************************************************************************
** Copyright 2015 GE Aviation Systems LLC, U.S.A.
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

#include <apexType.h>
#include <apexProcess.h>
#include <apexPartition.h>

#include <scoeAMIOEnable.h>

#include "QueuingPrinter.h"

static QUEUING_PORT_NAME_TYPE fQueuingPortName = "ConfigRequestQueuingReceiver";
//static MESSAGE_SIZE_TYPE maxMsgSize = 64;
static QueuingPrinter<32>* fQueuingPrinter;

//static QUEUING_PORT_ID_TYPE fQueuingPort;

/***************************************************************************************************
** ConfigSender
**  ARINC 653 process which sends a config message to the other partition
**
** Parameters:
**   None
**
** Returns:
**   Nothing
*/
// static void ConfigSender( void );

/***************************************************************************************************
** toImage
**  Returns the string representation of the specified return code.
**
** Parameters:
**   aReturnCode - The 653 procedure's return code
**
** Returns:
**   The string representation of aReturnCode
*/
//static const char* toImage( RETURN_CODE_TYPE aReturnCode );

/***************************************************************************************************
** MessagePrinter
**  ARINC 653 process which receives and prints a queuing and sampling message
**
** Parameters:
**  None
**
** Returns:
**  Nothing
*/
static void MessagePrinter( void );

// template<MESSAGE_SIZE_TYPE MessageContentMaxSize>


/***************************************************************************************************
** MessagePrinterCpp
**  The partition's entry point (called on cold and warm start). Creates processes/ports/etc then
**  transitions to NORMAL mode.
**
** Parameters:
**  None
**
** Returns:
**  Nothing
*/
extern "C" void droneController_main( void )
{
    RETURN_CODE_TYPE lReturnCode;
    PROCESS_ATTRIBUTE_TYPE lAttributes;
    PROCESS_ID_TYPE lProcessID;

    //QUEUING_PORT_ID_TYPE mPortID;

    scoeAmioEnable();
    
    strcpy( lAttributes.NAME, "MessagePrinter" );
    lAttributes.ENTRY_POINT = ( SYSTEM_ADDRESS_TYPE ) MessagePrinter;
    lAttributes.STACK_SIZE = 0x1000;
    lAttributes.BASE_PRIORITY = 4;
    lAttributes.PERIOD = ZERO_TIME_VALUE;
    lAttributes.TIME_CAPACITY = INFINITE_TIME_VALUE;
    lAttributes.DEADLINE = SOFT;

    CREATE_PROCESS( &lAttributes, &lProcessID, &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to create process" );
    }

    START( lProcessID, &lReturnCode );
    if ( lReturnCode != NO_ERROR )
    {
        printf( "Failed to start process" );
    }

    printf( "NORMAL2\n" );
    // RETURN_CODE_TYPE lArincReturn;
    // CREATE_QUEUING_PORT( fQueuingPortName, maxMsgSize + MII_HEADER_SIZE, 20, 
    //     DESTINATION, FIFO, &mPortID, &lArincReturn );
    // if ( lArincReturn != NO_ERROR )
    // {
    //     static APEX_BYTE sErrorMessage[] = "Failed to create queuing port";
    //     RAISE_APPLICATION_ERROR( APPLICATION_ERROR, sErrorMessage, 
    //         sizeof( sErrorMessage ) - 1, &lArincReturn );
    // }

    fQueuingPrinter = new QueuingPrinter<32>( fQueuingPortName, 20 );
    printf( "NORMAL\n" );
	SET_PARTITION_MODE( NORMAL, &lReturnCode );
    // SET_PARTITION_MODE should not return
    printf( "Failed to set partition to NORMAL" );

    // CREATE_QUEUING_PORT(
    //     "ConfigResponseQueuingSender",
    //     32,
    //     20,
    //     SOURCE,
    //     PRIORITY,
    //     &fQueuingPort,
    //     &lReturnCode );
    // if ( lReturnCode != NO_ERROR )
    // {
    //     printf( "Failed to create queuing port: %s", toImage( lReturnCode ) );
    // }

    printf("Created port\n");

    return;
}

/***************************************************************************************************
** MessagePrinter
*/
static void MessagePrinter( void )
{
    fQueuingPrinter->printMessage();
    STOP_SELF();
}




// uint8_t * formatTxMessage(char *txMsg){
//     size_t length = strlen(txMsg);
//     uint8_t* txByteMsg = new uint8_t[length];   //want null terminator? length + 1?
//     memcpy(txByteMsg, txMsg, length);
//     return txByteMsg;
// }

// /***************************************************************************************************
// ** toImage
// */
// static const char* toImage( RETURN_CODE_TYPE aReturnCode )
// {
//     const char* lResult = "UNKNOWN";
//     switch ( aReturnCode )
//     {
//     case NO_ERROR:
//         lResult = "NO_ERROR";
//         break;
//     case NO_ACTION:
//         lResult = "NO_ACTION";
//         break;
//     case NOT_AVAILABLE:
//         lResult = "NOT_AVAILABLE";
//         break;
//     case INVALID_PARAM:
//         lResult = "INVALID_PARAM";
//         break;
//     case INVALID_CONFIG:
//         lResult = "INVALID_CONFIG";
//         break;
//     case INVALID_MODE:
//         lResult = "INVALID_MODE";
//         break;
//     case TIMED_OUT:
//         lResult = "TIMED_OUT";
//         break;
//     }

//     return lResult;
// }