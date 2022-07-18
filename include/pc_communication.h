#ifndef PC_COMMUNICATION_H
#define PC_COMMUNICATION_H

#include <arduino.h>

constexpr uint32_t HANDSHAKE_LEN = 4;
constexpr char HANDSHAKE_VAL = 0xFF;
extern char HANDSHAKE_ARRAY[HANDSHAKE_LEN];

class pc_communication
{
public:
    
    // getters/setters
    bool get_handshake_received(void) const;
    bool get_pc_connected(void) const;

    void set_handshake_receieved(void);
    void set_pc_connected(void);

    void clear_handshake_receieved(void);
    void clear_pc_connected(void);


    // Constructor just sets handshake_receieved and pc_connected to false
    pc_communication(void);

    /*
        Description: checks if we're connected to the PC
        Returns:
            True if we are conected, o.w. false
    */
    bool is_connected(void);

    /*
        Description: Will attempt a handshake with the pc. Handshake is over UART so will be in chunks of 1 byte.
        Handshake length (in B) and value (a constant) are defined in the header file.
        
        Input Parameters:
            Length of timout in milliseconds
        Returns:
            True if we read in the correct sequence of bytes, false if we hit timeout durration
    */ 
    bool receive_handshake(void);

    /*
        Description: Sends the defined handshake value, and length over UART to the PC
    */ 
    void acknowledge_handshake(void);

    /*
        Description: checks if byte is in Rx fifo then if so reads it and echos it back to pc by setting the ACK byte in the data stream
    */
   char check_command_received(void);

    /*
        Description: will flush the receive buffer for the UART
    */
   void flush_rx_fifo(void);

private:
    bool handshake_received;
    bool pc_connected;

};

#endif // PC_COMMUNICATION_H