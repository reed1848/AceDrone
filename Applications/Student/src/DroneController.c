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

#include <apexType.h>
#include <apexProcess.h>
#include <apexPartition.h>
#include <apexQueuing.h>
#include <apexSampling.h>
#include "apex/apexLib.h"

#include <scoeAMIOEnable.h>

#include "DroneController.h"

#define MAX_RX_CONFIG_MSGS 16
#define MAX_MSG_SIZE 0xFF
#define PORT_TIMEOUT 10000000 // 100ms timeout

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
    QUEUING_PORT_NAME_TYPE portName = "ConfigRequestQueuingReceiver";
    QUEUING_PORT_ID_TYPE port_id;
    RETURN_CODE_TYPE return_code;
    MESSAGE_SIZE_TYPE length;
    uint8_t rxConfigBuffer[MAX_MSG_SIZE];
    scoeAmioEnable();

    // Step 1: Get the Queue Port ID from where to receive messages from
    GET_QUEUING_PORT_ID (portName, &port_id, &return_code);
    if (return_code != NO_ERROR)
    {
        printf("ERROR: Could not find specified port\n");
        return -1;
    }
    printf("Connected to receive port\n");

    // Step 2: Receive Messages from opened port
    for (int i = 0; i < MAX_RX_CONFIG_MSGS; i++)
    {
        void RECEIVE_QUEUING_MESSAGE (
            port_id,
            PORT_TIMEOUT,
            &rxMSGBuffer,
            &length,
            &return_code);
        if (return_code == NO_ERROR)
        {
            proccessRXConfig(rxMSGBuffer, length);
        }
        else if (return_code == EMPTY)
        {
            break;
        }
        else
        {
            printf("Error in receiving messages from port/queue");
            return -1;
        }
    }

    /* TODO: Proccess, format, and send configuration response back to OS here */

    SET_PARTITION_MODE( NORMAL, &return_code );
    if (return_code != NO_ERROR)
    {
        printf("ERROR: Could not set parition to NORMAL mode\n");
        return -1;
    }
 
} 

/**
 * Reads the configuration parameters from the OS from the specified port
 * 
 * Parameters:
 *     uint8_t *rxMsg
 *     MESSAGE_SIZE_TYPE length
 *      TODO: Add any more params here
 */
int proccessRXConfig(uint8_t *rxMsg, MESSAGE_SIZE_TYPE length) 
{
    /* TODO: Proccess Message here */
    printf("Received message %.*s\n", (int)length, rxMsg);
    return 0;
}


