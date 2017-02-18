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

TCPPacketConn& TCPPacketConn::operator=(TCPPacketConn&& other)
{
  fd = other.fd;
  other.fd = -1;
  receiver = other.receiver;
  
  delete[] readQueue;
  readQueue = other.readQueue;
  other.readQueue = NULL;
  readQueueSize = other.readQueueSize;
  readQueueBegin = other.readQueueBegin;
  currPacketLen = other.currPacketLen;
  currRecvLen = other.currRecvLen;
  headerRecv = other.headerRecv;
  headerRaw = other.headerRaw;
  startTime = other.startTime;

  delete[] writeQueue;
  writeQueue = other.writeQueue;
  other.writeQueue = NULL;
  writeQueueSize = other.writeQueueSize;
  writeQueueBegin = other.writeQueueBegin;
  writeQueueLen = other.writeQueueLen;
}

int TCPPacketConn::getFd() const
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
		printf("%02X", readQueue[(readQueueBegin + i) % readQueueSize]);
	}
  
    if(receiver != NULL)
	{
	  //receiver(req);
	  LOG(DEBUG) << "Packet Received";
	}
    else
      {
	LOG(WARN) << "No packet receiver registered." << fd;
      }
}

void TCPPacketConn::handleTimeout()
{
  //Might want to send some keep alive packets to resync
  LOG(WARN) << "Packet timed out";
}

bool TCPPacketConn::readyToWrite() const
{
  return (writeQueueLen > 0);
}

void TCPPacketConn::readData()
{
  std::lock_guard<std::mutex> lockGuard(queueMutex);
  uint8_t buffer[DEFAULTQUEUESIZE];
  int status = recv(fd, buffer, DEFAULTQUEUESIZE, 0);
  if(status == -1)
    {
      LOG(ERROR) << "Error reading fd " << fd << ": " << strerror(errno);
      throw CommunicationException("Error reading fd");
    }

  if(status == 0)
    {
      LOG(ERROR) << "Error with connection " << fd;
      throw ConnectionException("Error with connection");
    }


  if((currRecvLen + status) > readQueueSize)
    {
      readQueue = resizeQueue(readQueue, &readQueueSize, &readQueueBegin, currRecvLen, (currRecvLen + status));
    }

  for(int i = 0; i < status; i++)
    {
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
	      LOG(DEBUG) << "CurrPacketLen: " << currPacketLen;
	      headerRecv++;
	    }

	  //LOG(DEBUG) << "Receiving header";
	}
      else
	{
	  readQueue[(readQueueBegin + currRecvLen) % readQueueSize] = buffer[i];
	  currRecvLen++;
	}

      LOG(DEBUG) << "CurrRecvLen: " << currRecvLen;
      if((currRecvLen == currPacketLen) & (currRecvLen != 0)) //If we got the whole packet
	{
	  LOG(DEBUG) << "Received packet";
	  handlePacket();
	  readQueueBegin =(readQueueBegin + currPacketLen) % readQueueSize;
	  headerRecv = 0;
	  headerRaw = 0;
	  currRecvLen = 0;
	  currPacketLen = 0;
	}

      //LOG(DEBUG) << "Diff time: " << difftime(time(NULL), startTime);
      if(difftime(time(NULL), startTime) >= TIMEOUT) //If the timeout kicks in, the connection will have to resync
	{
	  handleTimeout();
	  readQueueBegin = (readQueueBegin + currRecvLen) % readQueueSize;
	  headerRecv = 0;
	  headerRaw = 0;
	  currRecvLen = 0;
	  currPacketLen = 0;
	}
    }
}


void TCPPacketConn::writeData()
{
  std::lock_guard<std::mutex> lockGuard(queueMutex);
  uint8_t buffer[writeQueueSize];
  for(int i = 0; i < writeQueueLen; i++)
    {
      buffer[i] = writeQueue[(writeQueueBegin + i) % writeQueueSize];
    }
  
  int status = send(fd, buffer, writeQueueLen, 0);
  if(status == -1)
    {
      LOG(ERROR) << "Error writing to socket " << fd << ": " << strerror(errno);
      throw CommunicationException("Error writing to socket");
    }

  writeQueueBegin = (writeQueueBegin + status) % writeQueueSize;
  writeQueueLen -= status;
}


void TCPPacketConn::insertData(const uint8_t *buffer, int len)
{
  std::lock_guard<std::mutex> lockGuard(queueMutex);
  if((writeQueueLen + len) > writeQueueSize)
    {
      writeQueue = resizeQueue(writeQueue, &writeQueueSize, &writeQueueBegin, writeQueueLen, writeQueueLen + len);
    }

  for(int i = 0; i < len; i++)
    {
      writeQueue[(writeQueueBegin + i) % writeQueueSize] = buffer[i];
    }
}

void TCPPacketConn::sendRequest(const RequestObj& req)
{
  LOG(DEBUG) << "Writing Packet";
}

void TCPPacketConn::setRequestReceiver(RequestReceiver r)
{
  receiver = r;
}

bool TCPPacketConn::isAuthenticated() const
{
  return authenticated;
}
