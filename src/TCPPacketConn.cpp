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

#include "TCPPacketConn.h"

TCPPacketConn::TCPPacketConn(int f) : fd(f)
{

  readQueue = new uint8_t [DEFAULTQUEUESIZE];
  readQueueSize = DEFAULTQUEUESIZE;
  readQueueBegin = 0;
  
  writeQueue = new uint8_t [DEFAULTQUEUESIZE];
  writeQueueSize = DEFAULTQUEUESIZE;
  writeQueueBegin = 0;
  writeQueueLen = 0;

  currRecvLen = 0;
  headerRecv = 0;
  headerRaw = 0;
}

TCPPacketConn::TCPPacketConn(const TCPPacketConn& in)
{
  fd = in.fd;
  authenticated = in.authenticated.load();
  receiver = in.receiver;

  readQueue = new uint8_t [in.readQueueSize];
  readQueueSize = in.readQueueSize;
  readQueueBegin = in.readQueueBegin;
  for(unsigned int i = 0; i < readQueueSize; i++)
    {
      readQueue[i] = in.readQueue[i];
    }
  
  currPacketLen = in.currPacketLen;
  currRecvLen = in.currRecvLen;
  headerRecv = in.headerRecv;
  headerRaw = in.headerRaw;
  startTime = in.startTime;

  writeQueue = new uint8_t [in.writeQueueSize];
  writeQueueSize = in.writeQueueSize;
  writeQueueBegin = in.writeQueueBegin;
  writeQueueLen = in.writeQueueLen;
  for(unsigned int i = 0; i < writeQueueSize; i++)
    {
      writeQueue[i] = in.writeQueue[i];
    }
}

TCPPacketConn::~TCPPacketConn()
{

  delete[] readQueue;
  delete[] writeQueue;

}

int TCPPacketConn::getFd()
{
  return fd;
}



uint8_t* TCPPacketConn::resizeQueue(uint8_t *queue, unsigned int *capacity, unsigned int *cursor, int len, int requestedLen)
{
  unsigned int newCapacity = requestedLen * 1.5;
  uint8_t *tmp = new uint8_t [newCapacity];
  unsigned int newCursor = 0;
  for(int i = 0; i < len; i++)
    {
      tmp[i] = queue[(*cursor + i) % *capacity];
    }

  delete[] queue;
  *capacity = newCapacity;
  *cursor = newCursor;
  return tmp;
}


void TCPPacketConn::handlePacket()
{
	std::vector<uint8_t> req;
	for(int i = 0; i < currRecvLen; i++)
	{
		req.push_back(readQueue[(readQueueBegin + i) % readQueueSize]);
	}
  
    if(receiver != NULL)
	{
	  receiver(req);
	}
	else
    {
      LOG(WARN) << "No packet receiver registered.";
    }
}

void TCPPacketConn::handleTimeout()
{
  //Might want to send some keep alive packets to resync
  LOG(WARN) << "Packet timed out";
}

bool TCPPacketConn::readyToWrite()
{
  return (writeQueueLen > 0);
}
