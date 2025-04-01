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
#include <map>

#include "DroneController.h"
#include "ConfigValidator.h"

#define MAX_RX_CONFIG_MSGS 16
#define MAX_MSG_SIZE 0xFF
#define PORT_TIMEOUT 10000000 // 100ms timeout

std::map<std::string, std::string> gConfigTable;
ConfigSpec gConfigSpec;

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
    //set up config receiving port
    QUEUING_PORT_NAME_TYPE portName = "ConfigRequestQueuingReceiver";
    QUEUING_PORT_ID_TYPE port_id;
    RETURN_CODE_TYPE return_code;
    MESSAGE_SIZE_TYPE length;
    uint8_t rxConfigBuffer[MAX_MSG_SIZE];
    scoeAmioEnable();

    //init Config Related variables
    gConfigSpec = init_config_spec();

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

    //set up config sending port
    QUEUING_PORT_NAME_TYPE sedingPortName = "ConfigRequestQueuingReceiver";
    QUEUING_PORT_ID_TYPE send_port_id;
    MESSAGE_SIZE_TYPE length;
    uint8_t txConfigBuffer[MAX_MSG_SIZE];

    // Step 3: Create Queuing Port and send config back to grader application
    GET_QUEUING_PORT_ID (sedingPortName, &port_id, &return_code);
    if (return_code != NO_ERROR)
    {
        printf("ERROR: Could not find specified sending port\n");
        return -1;
    }
    printf("Connected to sending port\n");

    sendStoredConfigData();

    //Step 4. Set Partition to Normal
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
    char **param_id, **value;
    char *inputConfigLine = copyRxMessage(rxMsg, (int)length);
    printf("Received message %.*s\n", (int)length, rxMsg);
    //parse message
    parse_config_message(inputConfigLine, param_id, value);
    ConfigValue *value = validate_config_message(gConfigSpec, *param_id, *value);

    //store config value
    if(value->type != INVALID){
        gConfigTable[*param_id] = std:to_string(*value);
    }

    //validate message
    return 0;
}

char * copyRxMessage(uint8_t *rxMsg, int length){
    char *message = (char*)malloc(length+1);
    if(message == NULL){
        return NULL;
    }
    memcpy(message, data, length);
    message[length] = '\0';
    return message;
}

uint8_t * formatTxMessage(char *txMsg){
    size_t length = strlen(txMsg);
    uint8_t* txByteMsg = new uint8_t[length];   //want null terminator? length + 1?
    memcpy(txByteMsg, txMsg, length);
    return txByteMsg;
}


void sendStoredConfigData(QUEUING_PORT_NAME_TYPE port, QUEUING_PORT_ID_TYPE port_id, MESSAGE_SIZE_TYPE length){
    uint8_t txConfigBuffer[MAX_MSG_SIZE];
    RETURN_CODE_TYPE return_code;

    // Send start tag
    char *start_tag = "Start - ConfigResponse"
    uint8_t start = formatTxMessage(start_tag);
    void SEND_QUEUING_MESSAGE (
        port_id,
        start_tag,
        &length,
        &return_code);

    //send end tag

    char *temp;
    for (const auto& entry : myMap) {
        temp += entry.first + ":" + entry.second + "\n";
        txConfigBuffer = formatTxMessage(temp);
        void SEND_QUEUING_MESSAGE (
            port_id,
            &txMSGBuffer,
            &length,
            &return_code);
    
    }

    // Send end tag
    char *end_tag = "End - ConfigResponse"
    uint8_t start = formatTxMessage(start_tag);
    void SEND_QUEUING_MESSAGE (
        port_id,
        end_tag,
        &length,
        &return_code);
}


