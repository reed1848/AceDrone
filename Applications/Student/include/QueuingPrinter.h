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

#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <iostream>
#include <cstring>
#include <unordered_map>


#include <apexType.h>
#include <apexError.h>
#include <apexQueuing.h>

#include <scoeMiiHeader.h>

template<MESSAGE_SIZE_TYPE MessageContentMaxSize>
class QueuingPrinter

#define NUM_CONFIG_ITEMS 15
#define MAX_PARAM_ID_LEN 20
#define MAX_VALUE_LEN 20
#define NUM_STRING_VALUES 5 //todo: less messy?


{
public:

    QUEUING_PORT_ID_TYPE mPortID;
    std::unordered_map<std::string, std::string> msgValues;
    /***********************************************************************************************
    ** QueuingPrinter
    **  Constructs a queuing printer object
    **
    ** Parameters:
    **  aName - Queuing port name
    **  aQueueSize - Maximum number of messages in the queue
    */
    QueuingPrinter( QUEUING_PORT_NAME_TYPE aName, MESSAGE_RANGE_TYPE aQueueSize, MESSAGE_SIZE_TYPE maxMsgSize)
    {
        printf( "NORMAL2\n" );
        RETURN_CODE_TYPE lArincReturn;
        CREATE_QUEUING_PORT( aName, maxMsgSize + MII_HEADER_SIZE, aQueueSize, 
            DESTINATION, FIFO, &mPortID, &lArincReturn );
        if ( lArincReturn != NO_ERROR )
        {
            static APEX_BYTE sErrorMessage[] = "Failed to create queuing port";
            RAISE_APPLICATION_ERROR( APPLICATION_ERROR, sErrorMessage, 
                sizeof( sErrorMessage ) - 1, &lArincReturn );
        }
        return;
    }
    
    /***********************************************************************************************
    ** printMessage
    **  Reads and prints a message from the queuing port
    **
    ** Parameters:
    **  None
    **
    ** Returns:
    **  Nothing
    */
    void printMessage( void )
    {
        printf( "NORMAL3\n" );
        RETURN_CODE_TYPE lArincReturn;
        MESSAGE_SIZE_TYPE lLength;
        APEX_BYTE lReceiveBuffer[64 + MII_HEADER_SIZE + 1] = {};
        int i = 0;
        while (i < 20){
            
            RECEIVE_QUEUING_MESSAGE( mPortID, INFINITE_TIME_VALUE, lReceiveBuffer, &lLength, 
                &lArincReturn );
            
            if ( lArincReturn != NO_ERROR )
            {
                static APEX_BYTE sErrorMessage[] = "Failed to receive queuing message";      
                RAISE_APPLICATION_ERROR( APPLICATION_ERROR, sErrorMessage, 
                    sizeof( sErrorMessage ) - 1, &lArincReturn );
            }
            printf( "%.*s\n", lLength - MII_HEADER_SIZE, &lReceiveBuffer[MII_HEADER_SIZE] );
            proccessRXConfig(lReceiveBuffer, lLength);
            //size_t colonPos = parse_config_message(lReceiveBuffer, lLength);
            //printf("%d\n", colonPos);
        }
        return;
    }

        /***********************************************************************************************
    ** sendMessage
    **  Reads and prints a message from the queuing port
    **
    ** Parameters:
    **  None
    **
    ** Returns:
    **  Nothing
    */
   void sendMessage( void )
    {
        RETURN_CODE_TYPE lReturnCode;
        char* lQueuingMessage;
        // const char* lSamplingMessage = "Hello Sampling!";

        const char* lQueuingMessageStart = "Start - Config Response";
        const char* lQueuingMessageEnd = "End - Config Response";
        
        SEND_QUEUING_MESSAGE( 
            mPortID,
            (MESSAGE_ADDR_TYPE) lQueuingMessageStart, 
            strlen( lQueuingMessageStart ) + 1, 
            INFINITE_TIME_VALUE,
            &lReturnCode );
        if ( lReturnCode != NO_ERROR )
        {
            static APEX_BYTE sErrorMessage[] = "Failed to receive queuing message";      
            RAISE_APPLICATION_ERROR( APPLICATION_ERROR, sErrorMessage, 
                sizeof( sErrorMessage ) - 1, &lReturnCode );
        }
        for (const auto& pair : msgValues)
        {
            char m[pair.first.length() + pair.second.length() + 1];
            sprintf(m, "%s:%s", pair.first, pair.second);
            SEND_QUEUING_MESSAGE( 
                mPortID,
                (MESSAGE_ADDR_TYPE) m, 
                strlen( lQueuingMessage ) + 1, 
                INFINITE_TIME_VALUE,
                &lReturnCode );
            if ( lReturnCode != NO_ERROR )
            {
                static APEX_BYTE sErrorMessage[] = "Failed to receive queuing message";      
                RAISE_APPLICATION_ERROR( APPLICATION_ERROR, sErrorMessage, 
                    sizeof( sErrorMessage ) - 1, &lReturnCode );
            }
        }
        SEND_QUEUING_MESSAGE( 
            mPortID,
            (MESSAGE_ADDR_TYPE) lQueuingMessageEnd, 
            strlen( lQueuingMessageEnd ) + 1, 
            INFINITE_TIME_VALUE,
            &lReturnCode );
        if ( lReturnCode != NO_ERROR )
        {
            static APEX_BYTE sErrorMessage[] = "Failed to receive queuing message";      
            RAISE_APPLICATION_ERROR( APPLICATION_ERROR, sErrorMessage, 
                sizeof( sErrorMessage ) - 1, &lReturnCode );
        }
        return;
    }

    // size_t parse_config_message(const APEX_BYTE* message, MESSAGE_SIZE_TYPE length) {
    //     // Find the position of the colon
    //     printf( "%.*s\n", length - MII_HEADER_SIZE, &message[MII_HEADER_SIZE] );
    //     std::string str;
    //     for (int i = 0; i < length; i++)
    //     {
    //         str += static_cast<char>(message[i + MII_HEADER_SIZE]);
    //         if (message[i] == ':')
    //         {
    //             printf("%s\n", str);
    //             return i - MII_HEADER_SIZE;
    //         }
    //     }
    //     return -1;
    // }
    
    float fuelUsageRate(std::unordered_map<std::string, float> m)
    {
        float g = 0.0314f;
        float l = 150.0f;
        float t = 0.0005f;
        float d = 0.000278f;
        float k = 0.9f;

        return (g * (m["Mass"] + (m["Capacity"] * l * t)) * m["Vel"] * d * k)/m["MPG"];
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
        // char **param_id = NULL;
        // char **value = NULL;
        printf( "%.*s\n", length - MII_HEADER_SIZE, &rxMsg[MII_HEADER_SIZE] );
        
        char *inputConfigLine = copyRxMessage(rxMsg, length);
        printf("Received message %.*s\n", (int)length, inputConfigLine);
        free(inputConfigLine);
        //parse message
        //parse_config_message(inputConfigLine, param_id, value);
        // ConfigValue *value = validate_config_message(gConfigSpec, *param_id, *value);

        // //store config value
        // if(value->type != INVALID){
        //     gConfigTable[*param_id] = std:to_string(*value);
        // }

        // //validate message
        return 0;
    }

    char * copyRxMessage(uint8_t *rxMsg, int length){
        printf( "%.*s\n", length - MII_HEADER_SIZE, &rxMsg[MII_HEADER_SIZE] );
        char *message = (char*)malloc((5+1) * sizeof(char));
        if(message == NULL){
            return NULL;
        }
        // uint8_t m[5];
        // memset(m, 't', sizeof(m));
        printf("%d\n", length - MII_HEADER_SIZE);
        memcpy(message, &rxMsg[MII_HEADER_SIZE], length - MII_HEADER_SIZE);
        message[length - MII_HEADER_SIZE] = '\0';
        
        return message;
    }
private:
    
};