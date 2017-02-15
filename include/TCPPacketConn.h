/*
  TCPConnObj Class
    Copyright (C) 2017 Scott McKittrick
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Created by: Scott McKittrick, Feb. 3rd 2017
*/

/*
  TCPConnObj is intended to hold read and write queues for each connection and provide packet framing.
	Packet: 
		----------------------------------------------
		| length 2 bytes | data                      |
		----------------------------------------------
  */

//Need to add keepalive function

#ifndef TCPPacketConn_H
#define TCPPacketConn_H

#include <queue>
#include <atomic>
#include <mutex>
#include <ctime>
#include <cstring>
#include <arpa/inet.h>
#include "CPPLogger.h"
#include "ServerExceptions.h"

#define TIMEOUT 2

#define DEFAULTQUEUESIZE 64
#define PACKETHEADERSIZE 2


class TCPPacketConn
{

 public:
  TCPPacketConn(int f);
  TCPPacketConn(const TCPPacketConn& in);
  ~TCPPacketConn();
  
  //Low level packet framing & control
  int getFd() const;
  bool readyToWrite() const;
  void readData();
  void writeData();

  
  //Higher level request object translation
  void sendRequest(const RequestObj& req);
  void setRequestReceiver(RequestReceiver r);
  bool isAuthenticated() const;
  

 private:
  int fd;
  
  //read and write queues are implemented as circular queues
  uint8_t *readQueue;
  unsigned int readQueueSize;
  unsigned int readQueueBegin;
  
  unsigned int currPacketLen; //Length of the current packet being read in
  unsigned int currRecvLen; //Amount of data already received
  unsigned int headerRecv; //The header is 4 bytes long, need to handle the case where we only recv part of it
  uint32_t headerRaw;
  time_t startTime; //Time this packet read began.
  
  uint8_t *writeQueue;
  unsigned int writeQueueSize;
  unsigned int writeQueueBegin;
  unsigned int writeQueueLen;
  void insertData(const uint8_t *buffer, int len);

  //resize queue
  static uint8_t* resizeQueue(uint8_t *queue, unsigned int *capacity, unsigned int *cursor, int len, int requestedLen);
  void handlePacket();
  void handleTimeout();
  
  RequestReceiver receiver;
  bool authenticated;
};
#endif
