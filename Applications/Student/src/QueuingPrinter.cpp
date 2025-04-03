#include "QueuingPrinter.h"

/***********************************************************************************************
** QueuingPrinter
**  Constructs a queuing printer object
**
** Parameters:
**  aName - Queuing port name
**  aQueueSize - Maximum number of messages in the queue
*/
QueuingPrinter::QueuingPrinter(MESSAGE_SIZE_TYPE messageSize, QUEUING_PORT_NAME_TYPE aName, MESSAGE_RANGE_TYPE aQueueSize)
{
    QueuingPrinter::gConfigSpec = init_config_spec();
    QueuingPrinter::messageContentSize = messageSize;
    //printf( "NORMAL2\n" );
    RETURN_CODE_TYPE lArincReturn;
    CREATE_QUEUING_PORT( aName, QueuingPrinter::messageContentSize + MII_HEADER_SIZE, aQueueSize, 
        DESTINATION, FIFO, &mPortID, &lArincReturn );
    if ( lArincReturn != NO_ERROR )
    {
        static APEX_BYTE sErrorMessage[] = "Failed to create queuing printer port";
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
   void QueuingPrinter::printMessage( void )
   {
       //printf( "NORMAL3\n" );
       RETURN_CODE_TYPE lArincReturn;
       MESSAGE_SIZE_TYPE lLength;
       APEX_BYTE lReceiveBuffer[QueuingPrinter::messageContentSize + MII_HEADER_SIZE + 1] = {};
       int i = 0;
       int response = 0;

       while ((i < 20) && (response == 0))
       {
           
        RECEIVE_QUEUING_MESSAGE( mPortID, INFINITE_TIME_VALUE, lReceiveBuffer, &lLength, 
            &lArincReturn );
           
           if ( lArincReturn != NO_ERROR )
           {
               static APEX_BYTE sErrorMessage[] = "Failed to receive queuing message";      
               RAISE_APPLICATION_ERROR( APPLICATION_ERROR, sErrorMessage, 
                   sizeof( sErrorMessage ) - 1, &lArincReturn );
           }
           response = proccessRXConfig(lReceiveBuffer, lLength);
           i++;
        }
       return;
   }

   /**
     * Reads the configuration parameters from the OS from the specified port
     * 
     * Parameters:
     *     uint8_t *rxMsg
     *     MESSAGE_SIZE_TYPE length
     *      TODO: Add any more params here
     */
    int QueuingPrinter::proccessRXConfig(uint8_t *rxMsg, MESSAGE_SIZE_TYPE length) 
    {
        // Create a stdString from input buffer
        std::string rxMsgString(reinterpret_cast<char*>(&rxMsg[MII_HEADER_SIZE]));

        // From the created string, search for seperator ':' and extract the key and value into a seperate string
        std::string key;
        std::string value;
        int colonPos = rxMsgString.find(':');
        key = rxMsgString.substr(0, colonPos);
        value = rxMsgString.substr(colonPos + 1);

        if((key != "Start") && (key != "End")){
            ConfigValue *configValue = validate_config_message(QueuingPrinter::gConfigSpec, key.c_str(), value.c_str());
            msgValues[key] = configValue->formated_value;
            printf("Validated Value: %s:%s\n", key.c_str(), configValue->formated_value);

        }
        else
        {
            if (key == "End")
            {
                return 1;
            }
        }
        return 0;
    }

    /**
     * Calculates fuel rate usage
     * 
     * Parameters:
     *     unordered map containing fuel usage params
     * 
     * Returns:
     *      float value containing fuel usage data
     */
    float QueuingPrinter::fuelUsageRate(std::unordered_map<std::string, float> m)
    {
        float g = 0.0314f;
        float l = 150.0f;
        float t = 0.0005f;
        float d = 0.000278f;
        float k = 0.9f;

        return (g * (m["Mass"] + (m["Capacity"] * l * t)) * m["Vel"] * d * k)/m["MPG"];
    }
