#include "QueuingSender.h"

/***********************************************************************************************
** QueuingSender
**  Constructs a queuing printer object
**
** Parameters:
**  aName - Queuing port name
**  aQueueSize - Maximum number of messages in the queue
*/
QueuingSender::QueuingSender(MESSAGE_SIZE_TYPE messageSize, QUEUING_PORT_NAME_TYPE aName, MESSAGE_RANGE_TYPE aQueueSize)
{
    QueuingSender::messageContentSize = messageSize;
    //printf( "NORMAL2\n" );
    RETURN_CODE_TYPE lArincReturn;
    CREATE_QUEUING_PORT( aName, QueuingSender::messageContentSize, aQueueSize, 
        SOURCE, FIFO, &mPortID, &lArincReturn );
    if ( lArincReturn != NO_ERROR )
    {
        static APEX_BYTE sErrorMessage[] = "Failed to create queuing send port";
        RAISE_APPLICATION_ERROR( APPLICATION_ERROR, sErrorMessage, 
            sizeof( sErrorMessage ) - 1, &lArincReturn );
    }
    return;
}

/***********************************************************************************************
    ** sendMessage
    **  Sends a message from the queuing port
    **
    ** Parameters:
    **  None
    **
    ** Returns:
    **  Nothing
    ** TODO: Needs to be tested
    */
   void QueuingSender::sendMessage( void )
   {
        RETURN_CODE_TYPE lReturnCode;
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
        // Iterate through all the values stored in the msgValues map
        for (const auto& pair : msgValues)
        {
            char m[pair.first.length() + pair.second.length() + 1];
            sprintf(m, "%s:%s", pair.first, pair.second);
            SEND_QUEUING_MESSAGE( 
                mPortID,
                (MESSAGE_ADDR_TYPE) m, 
                strlen( m ) + 1, 
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

