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
#include <stdlib.h>
#include <scoeAMIOEnable.h>
#include <sstream>
#include <string>

#include "DroneController.h"
#include "QueuingPrinter.h"
#include "QueuingSender.h"
#include "DistanceCalculator.h"

#define MAX_NUM_MSG 20

// Global Variables
std::unordered_map<std::string, std::string> msgValues;
static QUEUING_PORT_NAME_TYPE fQueuingPortName = "ConfigRequestQueuingReceiver";
static QUEUING_PORT_NAME_TYPE fQueuingSendPortName = "ConfigResponseQueuingSender";
static QueuingPrinter* fQueuingPrinter;
static QueuingSender* fQueuingSender;
static MESSAGE_SIZE_TYPE maxMsgSize = 32;

static void setInitialDistance(DistanceCalculator::IntialConfig* initialConfig);
static std::string convertArrayToString(const int* arr, size_t size);


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
DistanceCalculator::IntialConfig initalDistanceParameters;
extern "C" void droneController_main( void )
{
    scoeAmioEnable();

    RETURN_CODE_TYPE lArincReturn;

    fQueuingPrinter = new QueuingPrinter(maxMsgSize, fQueuingPortName, MAX_NUM_MSG);
    fQueuingSender = new QueuingSender(maxMsgSize, fQueuingSendPortName, MAX_NUM_MSG);
    printf( "Ports opened successfully!\n" );

    // Proccess the incoming messages
    fQueuingPrinter->printMessage();

    printf( "Input messages proccessed successfully\n" );

    // TODO: Call module to calculate distances here (probably fuel rates as well)?
    setInitialDistance(&initalDistanceParameters);

   DistanceCalculator dc(&initalDistanceParameters);

   for(int i = 0; i < dc.OBSTACLE_ALL; i++){
        int *distances = dc.GetDistance((DistanceCalculator::OBSTACLE_TYPE)i);
        char enumStr[20];
        dc.convertDistanceEnum((DistanceCalculator::OBSTACLE_TYPE)i, enumStr);
        printf("Size of Distances:%i\n", sizeof(distances));
        printf("Length of Distances:%i\n", sizeof(distances)/sizeof(int));

        std::string formatted = convertArrayToString(distances, (size_t)(sizeof(distances)/sizeof(int)));
        msgValues[enumStr] = formatted;
   }


    for (const auto& pair : msgValues) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }

    // TODO: Once info is proccessed correctly, call queuingSender to send response back to grader application and finish!
    
	SET_PARTITION_MODE( NORMAL, &lArincReturn );
    // SET_PARTITION_MODE should not return
    printf( "Failed to set partition to NORMAL" );

    return;
}

static std::string convertArrayToString(const int* arr, size_t size) {
    std::ostringstream oss;
    printf("%i", size);
    printf("In Progress: ");

    for (size_t i = 0; i < size; ++i) {
        printf("%i", arr[i]);
        if (i != 0) {
            oss << ", ";
        }
        oss << arr[i];
    }
    printf("\n%s\n", oss.str());
    return oss.str();
}

static void setInitialDistance(DistanceCalculator::IntialConfig* initialConfig)
{
    
    initialConfig->aRate = strtod(msgValues.at("Arate").c_str(), nullptr);
    initialConfig->bRate = strtod(msgValues.at("Brate").c_str(), nullptr);
    initialConfig->eRate = strtod(msgValues.at("Erate").c_str(), nullptr);
    initialConfig->mRate = strtod(msgValues.at("Mrate").c_str(), nullptr);
    initialConfig->sRate = strtod(msgValues.at("Srate").c_str(), nullptr);
    initialConfig->c1 = atoi(msgValues.at("C1").c_str());
    initialConfig->c2 = atoi(msgValues.at("C2").c_str());
    initialConfig->maxTime = atoi(msgValues.at("MaxTime").c_str());
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