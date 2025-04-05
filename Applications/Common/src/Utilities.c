#include <string.h>
#include <stdio.h>

#include "Utilities.h"

/***************************************************************************************************
** toImage
*/
const char* toImage( RETURN_CODE_TYPE aReturnCode )
{
    const char* lResult = "UNKNOWN";
    switch ( aReturnCode )
    {
    case NO_ERROR:
        lResult = "NO_ERROR";
        break;
    case NO_ACTION:
        lResult = "NO_ACTION";
        break;
    case NOT_AVAILABLE:
        lResult = "NOT_AVAILABLE";
        break;
    case INVALID_PARAM:
        lResult = "INVALID_PARAM";
        break;
    case INVALID_CONFIG:
        lResult = "INVALID_CONFIG";
        break;
    case INVALID_MODE:
        lResult = "INVALID_MODE";
        break;
    case TIMED_OUT:
        lResult = "TIMED_OUT";
        break;
    }

    return lResult;
}

RETURN_CODE_TYPE recvQueuingMsg( QUEUING_PORT_ID_TYPE aPortID
                               , MESSAGE_ADDR_TYPE aBuffer
                               , MESSAGE_SIZE_TYPE* aLenMsgData )
{
    RETURN_CODE_TYPE lReturnCode;
    char lTmpBuffer[MSG_BUFFER_SIZE] = {0};

    RECEIVE_QUEUING_MESSAGE(  aPortID,
                              0,
                              aBuffer,
                              aLenMsgData,
                              &lReturnCode );

    if( *aLenMsgData > 32 )
    {
        //Remove message header
        memcpy( lTmpBuffer, aBuffer+32, MSG_BUFFER_SIZE-32-1 );
        memcpy( aBuffer, lTmpBuffer, MSG_BUFFER_SIZE );
        *aLenMsgData -= 32;
    }

    return lReturnCode;
}

RETURN_CODE_TYPE recvQueuingBlockingMsg( QUEUING_PORT_ID_TYPE aPortID
                               , SYSTEM_TIME_TYPE aTimeOut
                               , MESSAGE_ADDR_TYPE aBuffer
                               , MESSAGE_SIZE_TYPE* aLenMsgData )
{
    RETURN_CODE_TYPE lReturnCode;
    char lTmpBuffer[MSG_BUFFER_SIZE] = {0};

    RECEIVE_QUEUING_MESSAGE(  aPortID,
                              aTimeOut,
                              aBuffer,
                              aLenMsgData,
                              &lReturnCode );

    if( *aLenMsgData > 32 )
    {
        //Remove message header
        memcpy( lTmpBuffer, aBuffer+32, MSG_BUFFER_SIZE-32-1 );
        memcpy( aBuffer, lTmpBuffer, MSG_BUFFER_SIZE );
        *aLenMsgData -= 32;
    }

    return lReturnCode;
}
