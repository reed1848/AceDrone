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

#ifndef QUEUINGPRINTER_H
#define QUEUINGPRINTER_H

#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <iostream>
#include <cstring>
#include <apexType.h>
#include <apexError.h>
#include <apexQueuing.h>

#include <scoeMiiHeader.h>

extern "C" {
#include "ConfigValidator.h"
}
#include "DroneController.h"

class QueuingPrinter
{
    public:
        // Public members
        QUEUING_PORT_ID_TYPE mPortID;

        // Constructor
        QueuingPrinter(MESSAGE_SIZE_TYPE messageSize, QUEUING_PORT_NAME_TYPE aName, MESSAGE_RANGE_TYPE aQueueSize );

        // Public Methods
        void printMessage( void );
        float fuelUsageRate(std::unordered_map<std::string, float> m);
        int proccessRXConfig(uint8_t *rxMsg, MESSAGE_SIZE_TYPE length);
        // size_t parse_config_message(const APEX_BYTE* message, MESSAGE_SIZE_TYPE length);
    private:
        // Private members
        MESSAGE_SIZE_TYPE messageContentSize;
        ConfigSpec *gConfigSpec;
        
    
};
#endif