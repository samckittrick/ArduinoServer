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
  TCPConnObj is intended to hold all of the data associated with a given connect.
  This includes the file descriptor, data to be sent and received, and authentication status */

#ifndef TCPPacketConn_H
#define TCPPacketConn_H

#include <queue>
#include <atomic>
#include <mutex>
#include <ctime>
#include <arpa/inet.h>
#include "RequestObj.h"
#include "CPPLogger.h"

#define TIMEOUT 2

#define DEFAULTQUEUESIZE 64
#define PACKETHEADERSIZE 2
#define PACKETDSTOFFSET 0
#define PACKETTYPEOFFSET 1
#define PACKETCMDOFFSET 2
#define PACKETDATAOFFSET 4

#define PACKETDATATYPE 0x01
#define PACKETAUTHTYPE 0x02


class TCPPacketConn
{

 public:
  TCPPacketConn(int f);
  TCPPacketConn(const TCPPacketConn& in);
  ~TCPPacketConn();
  
  //Accessors
  int getFd();
  bool getAuthenticated();
  void setAuthenticated(bool auth);
  void addRequest(const RequestObj& req);
  bool readyToWrite();
  //int getWriteData(uint8_t *packet, int size);
  void notifyWritten(int len);
  void insertData(const uint8_t *buffer, int len);

  //set request receiver
  void setRequestListener(RequestReceiver r);
  
  

 private:
  int fd;
  std::atomic<bool> authenticated;
  std::mutex RequestMutex;
  RequestReceiver receiver;

    //Data queues
  std::queue<RequestObj> reqQueue;
  
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

  //resize queue
  static uint8_t* resizeQueue(uint8_t *queue, unsigned int *capacity, unsigned int *cursor, int len, int requestedLen);
  void handlePacket();
  void handleTimeout();
  //void addPacketToWriteQueue();

};
#endif
