#include <iostream>
#include <string>
#include "TCPPacketConn.h"
#include "RequestObj.h"
#include "CPPLogger.h"


void receiveRequest(const RequestObj& req)
{
  LOG(DEBUG) << "Received Packet: " << req.toString();
}

int main()
{
  
  CPPLogger::getLog().setLogLevel(DEBUG);

  TCPPacketConn conn(5);
  conn.setRequestListener(receiveRequest);

  //std::string line;
  uint8_t data[] = { 0x00, 0x07, 0x05, 0x01, 0x00, 0x01, 0x45, 0x46, 0x47 };
  LOG(DEBUG) << "Sending Full packet:";

  conn.insertData(data, 9);

  //getline(std::cin, line);
  //sleep(5);
    LOG(DEBUG) << "Sending Full Packet again: ";
  conn.insertData(data, 9);

  //getline(std::cin, line);
  LOG(DEBUG) << "Sending Packet one at a time";
  for(int i = 0; i < 9; i++)
    {
      conn.insertData(data+i, 1);
    }

  LOG(DEBUG) << "Sending giant packet: ##############";
  uint8_t data2[] = {  0x00, 0x80 };
  conn.insertData(data2, 2);
  for(int i = 0; i < 128; i += 8)
    {
      uint8_t data3[] { 0x01, 0x01, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
      conn.insertData(data3, 8);
    }



}
