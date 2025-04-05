#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <apexType.h>
#include <apexQueuing.h>

#define MSG_BUFFER_SIZE 100

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
const char* toImage( RETURN_CODE_TYPE aReturnCode );

/***************************************************************************************************
** recvQueuingMsg
**  A wrapper around RECEIVE_QUEUING_MESSAGE which removes the 32 byte
**  message header when a message is received.
**
** Parameters:
**   aPortID - Queueing port to be used for receiveing the message
**   aBuffer - Buffer to store the received message
**   aLenMsgData (out) - Length of the message received (less the 32 byte header)
**
** Returns:
**   The 653 return code from the RECEIVE_QUEUING_MESSAGE call.
*/
RETURN_CODE_TYPE recvQueuingMsg( QUEUING_PORT_ID_TYPE aPortID
                               , MESSAGE_ADDR_TYPE aBuffer
                               , MESSAGE_SIZE_TYPE* aLenMsgData );


/***************************************************************************************************
** recvQueuingBlockingMsg
**  A wrapper around RECEIVE_QUEUING_MESSAGE which removes the 32 byte
**  message header when a message is received.
**
** Parameters:
**   aPortID - Queueing port to be used for receiveing the message
**   aTimeOut - Amount of time to wait for message to be received
**   aBuffer - Buffer to store the received message
**   aLenMsgData (out) - Length of the message received (less the 32 byte header)
**
** Returns:
**   The 653 return code from the RECEIVE_QUEUING_MESSAGE call.
*/
RETURN_CODE_TYPE recvQueuingBlockingMsg( QUEUING_PORT_ID_TYPE aPortID
                                        , SYSTEM_TIME_TYPE aTimeOut
                                        , MESSAGE_ADDR_TYPE aBuffer
                                        , MESSAGE_SIZE_TYPE* aLenMsgData );

#endif
