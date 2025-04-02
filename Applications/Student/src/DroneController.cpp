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
static QUEUING_PORT_NAME_TYPE fQueuingSendPortName = "ConfigResponseQueuingSender";

static QueuingPrinter<32>* fQueuingPrinter;
static QueuingPrinter<32>* fQueuingSender;

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


extern "C" void droneController_main( void )
{
    PROCESS_ATTRIBUTE_TYPE lAttributes;

    scoeAmioEnable();
    
    strcpy( lAttributes.NAME, "MessagePrinter" );
    lAttributes.ENTRY_POINT = ( SYSTEM_ADDRESS_TYPE ) MessagePrinter;
    lAttributes.STACK_SIZE = 0x1000;
    lAttributes.BASE_PRIORITY = 4;
    lAttributes.PERIOD = ZERO_TIME_VALUE;
    lAttributes.TIME_CAPACITY = INFINITE_TIME_VALUE;
    lAttributes.DEADLINE = SOFT;

    fQueuingPrinter = new QueuingPrinter<32>( fQueuingPortName, 20, 64);
    fQueuingSender = new QueuingPrinter<32>( fQueuingSendPortName, 20, 32);

    fQueuingPrinter->printMessage();
    fQueuingSender->sendMessage();

    printf("Created ports\n");

    return;
}

/***************************************************************************************************
** MessagePrinter
*/
static void MessagePrinter( void )
{
    //fQueuingPrinter->printMessage();
    STOP_SELF();
}