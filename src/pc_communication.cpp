#include <pc_communication.h>

char HANDSHAKE_ARRAY[HANDSHAKE_LEN] = {'\0'};

pc_communication::pc_communication(void)
{
    handshake_received = false;
    pc_connected = false;
}

// getters/setters/clearers?
bool pc_communication::get_handshake_received(void) const
{
    return handshake_received;
}

bool pc_communication::get_pc_connected(void) const
{
    return pc_connected;
}

void pc_communication::set_handshake_receieved(void)
{
    handshake_received = true;
}

void pc_communication::set_pc_connected(void)
{
    pc_connected = true;
}

void pc_communication::clear_handshake_receieved(void)
{
    handshake_received = false;
}

void pc_communication::clear_pc_connected(void)
{
    pc_connected = false;
}

/*
    Description: checks if we're connected to the PC and will set pc_connected if true
    Returns:
        True if we are conected, o.w. false
*/
bool pc_communication::is_connected(void)
{
    if(Serial) 
    {
        set_handshake_receieved();
        return true;
    }
    else {
        clear_handshake_receieved();
        return false;
    }
}

/*
    Description: Will attempt a handshake with the pc. Handshake is over UART so will be in chunks of 1 byte.
    Handshake length (in B) and value (a constant) are defined in the header file.
    
    Input Parameters:
        Length of timout in milliseconds
    Returns:
        True if we read in the correct sequence of bytes, false if we hit timeout durration
*/ 
bool pc_communication::receive_handshake(void)
{
    int counter = 0;
    if(Serial.available()) // num bytes available in input stream = handshake len; we're assuming they send the exact number
    {
        if (Serial.available() == 4)
        {
           Serial.readBytes(HANDSHAKE_ARRAY, 4);

           for(uint32_t i = 0; i < HANDSHAKE_LEN; i++)
           {
               if(HANDSHAKE_ARRAY[i] != HANDSHAKE_VAL) // If we hit a value that doesn't mach our check val then we clear the input stream then break out of loop
               {
                   Serial.write(0x00);
                   i = 5;
                   return false;
               }
               return true;
           }
        }
        else {
            while(Serial.available())
            {
                if(Serial.read() == 0xFF)
                {counter++;}
                else{
                    counter &= ~counter;
                }
                if(counter == 4)
                {
                    return true;
                }
            }
        }
        

    }
    else{
        Serial.println("4 or more bytes not available for receive_handshake");
    }
    return false;
}

/*
    Description: Sends the defined handshake value, and length over UART to the PC
*/ 
void pc_communication::acknowledge_handshake(void)
{
    while(!Serial.availableForWrite());
    set_handshake_receieved();
    if(Serial.availableForWrite()) // I could wrap the following for loops into 1 that sets, sends, then clears but I want everything to be sent at the closest interval possible to the PC
    {
            for(uint32_t i = 0; i < HANDSHAKE_LEN; i++) {
                HANDSHAKE_ARRAY[i] = HANDSHAKE_VAL; // set to handshake array to value
            }

            Serial.write(HANDSHAKE_ARRAY, HANDSHAKE_LEN);

        for(uint32_t i = 0; i < HANDSHAKE_LEN; i++) {
            HANDSHAKE_ARRAY[i] &= ~(HANDSHAKE_ARRAY[i]); // clear handshake array
        }
    }


}

/*
    Description: checks if byte is in Rx fifo then if so it will return the value
*/
char pc_communication::check_command_received(void)
{
    if(Serial.available() == 1)
    {
        char temp_byte = 0;
        Serial.readBytes(&temp_byte, 1);
        return temp_byte;   
    }
return 0;
}


/*
    Description: will flush the receive buffer for the UART
*/
void pc_communication::flush_rx_fifo(void)
{
    while(Serial.available())
    {
        Serial.read();       
    }
}
