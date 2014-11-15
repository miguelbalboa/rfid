/*
    rdtag_eth_UDP.ino  created by Matteo Facchetti

		 - Uses the library files MFRC522.h e MFRC522.cpp created by Miguel Balboa
		 - Uses the library files modMFRC.h e modMFRC.cpp created by Matteo Facchetti

    DESCRIPTION:
    This example allows you to read a tag and send data via UDP broadcast.

    The UDP packet should be formatted as:
    byte [0] = read function
    byte [1] = 0x0 // only applies to reply
    byte [2] ... byte [17] = 16 bytes data to send via broadcast UDP

	The application waits for the scan of a card.
	If the card is present and is read correctly (a block of 16 bytes), the data are sent via a UDP broadcast packet.
	Then it waits to receive a reply from remote, with a maximum timeout of 1.5 seconds. 

	If the response is received with the correct command and status,	
	print "Reply OK!" otherwise "ERROR reply or timeout."

	Eventually you can communicate the results of the reading and transmission of data via LEDs.

*/
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "modMFRC.h"

#define TIMEOUT_CMD_RD  1500

#define CMD_SEND_RD_TAG     0x1
#define CMD_REPLY_RD_TAG    0x2

#define EXEC_OK     1       // successful command
#define ERROR_CMD   0xff    // generic error

typedef struct
{
    uint8_t cmd;        // command id
    uint8_t sts;        // operation status (valid for the reply)
    uint8_t data[16];   // one block data
} STRUCT_PKT;

STRUCT_PKT send_struct;
STRUCT_PKT recv_struct;

uint8_t rd_tag_buf[16];

uint8_t status = 0;     // Command or TAG status
uint8_t read_tag = 1;   // Default Read TAG enable
uint32_t time_chk;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 15);
IPAddress subMask(255, 255, 255, 0);

uint16_t localPort = 12345;      // local port to listen on
uint16_t remotePort = 12345;      // remote port to send

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

// set my own key if necessary
byte ownkeyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //{0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF}; //

/*
    Function: sendUDPpacket(-)
    Description: Send UDP packet
*/
void sendUDPpacket(uint32_t remote_ip, uint16_t remote_port, uint8_t *data, uint16_t size)
{
    // Send udp data
    uint16_t cnt = 0;
    Udp.beginPacket(remote_ip, remote_port);
    while (cnt < size)
        Udp.write(data[cnt++]);
    Udp.endPacket();
    Serial.print("the packet was sent-> size:");
    Serial.println(cnt);
}

/*
    Function: sendUDPbroadcast(-)
    Description: Send UDP broadcast packet
*/
void sendUDPbroadcast(uint8_t *data, uint16_t size, uint16_t dport)
{
    // set network broadcast address and destination port
    sendUDPpacket(((ip & subMask) | ~subMask), dport, data, size);
}

/*
    Function: send_pkt(-)
    Description: create data struct to send
*/
void send_pkt(uint8_t cmd, uint8_t sts, uint8_t* data, uint8_t sz)
{
    memset(&send_struct, 0, sizeof(send_struct)); // clear del buffer
    send_struct.cmd = cmd;   // command
    send_struct.sts = sts;   // status
    memcpy(&send_struct.data[0], data, (sz > 16) ? 16 : sz); // force max 16 byte data (one block)
    sendUDPbroadcast((uint8_t *)&send_struct, sizeof(send_struct), remotePort);
}

/*
    Function: recvUDP(-)
    Description: receive UDP packet
*/
uint16_t recvUDP(uint8_t *data, uint16_t size)
{
    if (Udp.parsePacket())
        return(Udp.read(data, size));   // There is a packet
    else
        return (0);                     // No packet
}

/*
    Function: recv_pkt(-)
    Description: receive data struct
*/
uint16_t recv_pkt()
{
    int len = 0;
    memset(&recv_struct, 0, sizeof(recv_struct)); // clear del buffer

    // read the packet
    if ((len = recvUDP((uint8_t *)&recv_struct, sizeof(recv_struct))))
    {
        Serial.print("RX UDP-> len:");
        Serial.println(len);
    }
    return (len);
}

/*
    Function: setup(-)
    Description: Init application
*/
void setup()
{
    Serial.begin(9600);        // Initialize serial communications with the PC
    while(!Serial) ;

    SPI.begin();                // Init SPI bus

    initMFRC(ownkeyA, 0x0);    // set the key for replace default keyA and keyB

    // Do this every time you want to change the block/trailer to use
    // only once at initialization in this case
    setIdxBlk(1);
    setIdxTrailer(3);


    Ethernet.begin(mac, ip);
    Udp.begin(localPort);

    time_chk = 0L;
    Serial.println("Init Application");
}

/*
    Function: loop(-)
    Description: task
*/
void loop()
{
    uint32_t time = millis();

    if (read_tag)
    {
        // wait to read a TAG and send data-block
        if (status = check_rfid(AUTH_KEYA, MFRC_RD, rd_tag_buf, sizeof(rd_tag_buf)))// read data from card
        {
            // TAG presente
            print_tag_sts(status);
            if (status == MFRC_RD_OK)
            {
                send_pkt(CMD_SEND_RD_TAG, status, rd_tag_buf, sizeof(rd_tag_buf));     // send data
                read_tag = 0; // go to wait a UDP reply
                time_chk = time;
                delay(5);
            }
        }
    }
    else
    {
        // wait and handle UDP packet
        status = 0;    // sure is not an old status
        recv_pkt(); // continuous receive

        if(recv_struct.cmd == CMD_REPLY_RD_TAG)  // wait reply command
            status = recv_struct.sts;    // get status operation from packet

        // status or timeout generate a state change
        if ((status) || ((time - time_chk) > TIMEOUT_CMD_RD))
        {
            // Need a reply cmd with status 0x1

            // example packet
            // byte[0] = 0x2   command
            // byte[1] = 0x1   status ok
            // byte[2] ... byte [17] = reply data

            if (status == EXEC_OK)
                Serial.println("Reply OK!");
            else
                Serial.println("Reply ERROR or timeout!");

            read_tag = 1; // go to wait a UDP reply from server
            time_chk = 0L;
        }
    }
}
