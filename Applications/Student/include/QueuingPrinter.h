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
{
public:
    /***********************************************************************************************
    ** QueuingPrinter
    **  Constructs a queuing printer object
    **
    ** Parameters:
    **  aName - Queuing port name
    **  aQueueSize - Maximum number of messages in the queue
    */
    QueuingPrinter( QUEUING_PORT_NAME_TYPE aName, MESSAGE_RANGE_TYPE aQueueSize )
    {
        RETURN_CODE_TYPE lArincReturn;
        CREATE_QUEUING_PORT( aName, MessageContentMaxSize + MII_HEADER_SIZE, aQueueSize, 
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

        RETURN_CODE_TYPE lArincReturn;
        MESSAGE_SIZE_TYPE lLength;
        APEX_BYTE lReceiveBuffer[MessageContentMaxSize + MII_HEADER_SIZE + 1] = {};
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
            //size_t colonPos = parse_config_message(lReceiveBuffer, lLength);
            //printf("%d\n", colonPos);
        }
        return;
    }

    size_t parse_config_message(const APEX_BYTE* message, MESSAGE_SIZE_TYPE length) {
        // Find the position of the colon
        printf( "%.*s\n", length - MII_HEADER_SIZE, &message[MII_HEADER_SIZE] );
        std::string str;
        for (int i = 0; i < length; i++)
        {
            str += static_cast<char>(message[i + MII_HEADER_SIZE]);
            if (message[i] == ':')
            {
                printf("%s\n", str);
                return i - MII_HEADER_SIZE;
            }
        }
        return -1;
    }
    
    float fuelUsageRate(std::unordered_map<std::string, float> m)
    {
        float g = 0.0314f;
        float l = 150.0f;
        float t = 0.0005f;
        float d = 0.000278f;
        float k = 0.9f;

        return (g * (m["Mass"] + (m["Capacity"] * l * t)) * m["Vel"] * d * k)/m["MPG"];
    }
private:
    QUEUING_PORT_ID_TYPE mPortID;
};