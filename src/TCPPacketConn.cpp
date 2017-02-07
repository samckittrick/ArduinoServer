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

bool TCPPacketConn::getAuthenticated()
{
  return authenticated.load();
}

void TCPPacketConn::setAuthenticated(bool auth)
{
  authenticated = auth;
}

void TCPPacketConn::addRequest(const RequestObj& req)
{
  std::lock_guard<std::mutex> lockGuard(RequestMutex);
  reqQueue.push(req);
}

void TCPPacketConn::insertData(const uint8_t *buffer, int len)
{
  //LOG(DEBUG) << "Inserting Data";
  if((currRecvLen + len) > readQueueSize)
    {
      LOG(DEBUG) << "Resizing Queue";
      //LOG(DEBUG) << "CurrRecvLen: " << currRecvLen << " readQueueSize: " << readQueueSize;
      readQueue = resizeQueue(readQueue, &readQueueSize, &readQueueBegin, currRecvLen, currRecvLen + len);
    }

  for(int i = 0; i < len; i++)
    {
      //If we haven't received the full header yet
      if(headerRecv < PACKETHEADERSIZE)
	{
	  startTime = time(NULL);
	  
	  if(headerRecv == 0)
	    {
	      headerRaw = buffer[i] << 8;
	      headerRecv++;
	    }
	  else
	    {
	      headerRaw |= buffer[i];
	      //LOG(DEBUG) << "Raw Header: " << headerRaw;
	      currPacketLen = headerRaw;
	      //LOG(DEBUG) << "CurrPacketLen: " << currPacketLen;
	      headerRecv++;
	    }

	  //LOG(DEBUG) << "Receiving header";
	}
      else //If it's part of the packet, add it to the buffer 
	{
	  readQueue[(readQueueBegin + currRecvLen) % readQueueSize] = buffer[i];
	  currRecvLen++;

	  //LOG(DEBUG) << "Receiving data";
	}

      if(currRecvLen == currPacketLen) //If we got the whole packet
	{
	  LOG(DEBUG) << "Received packet";
	  handlePacket();
	  readQueueBegin =(readQueueBegin + currPacketLen) % readQueueSize;
	  headerRecv = 0;
	  currRecvLen = 0;
	}

      //LOG(DEBUG) << "Diff time: " << difftime(time(NULL), startTime);
      if(difftime(time(NULL), startTime) >= TIMEOUT) //If the timeout kicks in, the connection will have to resync
	{
	  handleTimeout();
	  readQueueBegin = (readQueueBegin + currRecvLen) % readQueueSize;
	  headerRecv = 0;
	  currRecvLen = 0;
	}
    }
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
  uint8_t dst = readQueue[readQueueBegin + PACKETDSTOFFSET];
  uint8_t type = readQueue[readQueueBegin + PACKETTYPEOFFSET];
  
  if(type == PACKETAUTHTYPE)
    {
      //handle Auth
      LOG(DEBUG) << "Auth Packet Received";
    }
  else if(type == PACKETDATATYPE)
    {
      LOG(DEBUG) << "Data Packet Received";
      //Package the data
      std::vector<uint8_t> d;
      //LOG(DEBUG) << "Data Len " << (currPacketLen - PACKETDATAOFFSET);
      //LOG(DEBUG) << "RecvLen " << currRecvLen;
      for(unsigned long i = PACKETDATAOFFSET; i < currRecvLen; i++)
	{
	  d.push_back(readQueue[i]);
	}

      //LOG(DEBUG) << "Data Size: " << d.size();
      uint16_t rawCmd = (readQueue[PACKETCMDOFFSET] << 8) + readQueue[PACKETCMDOFFSET + 1];
      RequestObj req(rawCmd, dst, (uint8_t)(fd&0xFF), d);
      if(receiver != NULL)
	{
	  receiver(req);
	}
    }
  else
    {
      LOG(WARN) << "Unknown packet type";
    }
}

void TCPPacketConn::handleTimeout()
{
  //Might want to send some keep alive packets to resync
  LOG(WARN) << "Packet timed out";
}

void TCPPacketConn::setRequestListener(RequestReceiver r)
{
  receiver = r;
}

bool TCPPacketConn::readyToWrite()
{
  return (writeQueueLen > 0) | (reqQueue.size() > 0);
}

void TCPPacketConn::notifyWritten(int len)
{
  writeQueueBegin = (writeQueueBegin + len) % writeQueueSize;
  writeQueueLen -= len;
}

/*void TCPPacketConn::addPacketToWriteQueue()
{
  if(writeQueueLen == 0 && reqQueue.size() > 0)
    {
      RequestObj req = reqQueue.front();
      reqQueue.pop();
      
      //3 is the size of the device id + command
      int packetLen = PACKETHEADERSIZE + 3 + req.getData().size();
      if(packetLen > writeQueueSize)
	{
	  resizeQueue(writeQueue, &writeQueueSize, &writeQueueBegin, writeQueueLen, packetLen);
	}
      
      //Put in the packet length
      writeQueue[(writeQueueBegin + writeQueueLen) % writeQueueSize] = packetLen >> 8;
      writeQueueLen++;
      writeQueue[(writeQueuBegin + writeQueueLen) % writeQueueSize] = packetLen & 0xFF;
      writeQueueLen++;

      //Put in the devId
      writeQueue[(writeQueueBegin + writeQueueLen) % writeQueueSize] = req.getSrc();
      writeQueueLen++;

      //put in the type*/
