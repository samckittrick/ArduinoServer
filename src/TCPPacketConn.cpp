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

//TCPPacketConn provides packet framing for an individual connection

#include "TCPPacketConn.h"

TCPPacketConn::TCPPacketConn(int f) : fd(f)
{

  //LOG(DEBUG) << "Constructor";

  readQueue = new uint8_t [DEFAULTQUEUESIZE];
  readQueueSize = DEFAULTQUEUESIZE;
  readQueueBegin = 0;
  readQueueLen = 0;

  writeQueue = new uint8_t [DEFAULTQUEUESIZE];
  writeQueueSize = DEFAULTQUEUESIZE;
  writeQueueBegin = 0;
  writeQueueLen = 0;

  headerRecv = 0;
  headerRaw = 0;
}

TCPPacketConn::TCPPacketConn(const TCPPacketConn& in)
{
  //LOG(DEBUG) << "Packet Conn Copy Constructor";
  fd = in.fd;
  //receiver = in.receiver;

  readQueue = new uint8_t [in.readQueueSize];
  readQueueSize = in.readQueueSize;
  readQueueBegin = in.readQueueBegin;
  readQueueLen = in.readQueueLen;
  for(unsigned int i = 0; i < readQueueSize; i++)
    {
      readQueue[i] = in.readQueue[i];
    }
  
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
  LOG(DEBUG) << "Move Assignment operator";
  fd = other.fd;
  other.fd = -1;
  //receiver = other.receiver;
  
  delete[] readQueue;
  readQueue = other.readQueue;
  other.readQueue = NULL;
  readQueueSize = other.readQueueSize;
  readQueueBegin = other.readQueueBegin;
  readQueueLen = other.readQueueLen;
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


void TCPPacketConn::handlePacket(std::vector<uint8_t>& data)
{
	for(int i = 0; i < headerRaw; i++)
	{
		data.push_back(readQueue[(readQueueBegin + i) % readQueueSize]);
		//printf("%02X", readQueue[(readQueueBegin + i) % readQueueSize]);
	}
	//Remove the packet from the queue
	readQueueBegin = (readQueueBegin + headerRaw) % readQueueSize;
	readQueueLen -= headerRaw;
	headerRecv = 0;
}

void TCPPacketConn::handleTimeout()
{
  //Might want to send some keep alive packets to resync
  LOG(WARN) << "Packet timed out";
  //Empty the queue
  headerRecv = 0;
  readQueueBegin = (readQueueBegin + readQueueLen) % readQueueSize;
  readQueueLen = 0;
}

void TCPPacketConn::handleKeepAlive()
{
  LOG(WARN) << "Keep alive request received.";
  headerRecv = 0;
}

bool TCPPacketConn::readyToWrite() const
{
  //LOG(DEBUG) << "Are we ready to write?";
  //LOG(DEBUG) << "WriteQueueLen for fd " << fd << " is " << writeQueueLen;
  return writeQueueLen > 0;
}

void TCPPacketConn::readData()
{
  //LOG(DEBUG) << "Reading Data";
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


  if((readQueueLen + status) > readQueueSize)
    {
      readQueue = resizeQueue(readQueue, &readQueueSize, &readQueueBegin, readQueueLen, (readQueueLen + status));
    }

  for(int i = 0; i < status; i++)
    {
      readQueue[(readQueueBegin + i) % readQueueSize] = buffer[i];
      readQueueLen++;
    }
}

int TCPPacketConn::processData(std::vector<uint8_t>& packet)
{
  //LOG(DEBUG) << "Processing Data";
  //Process the header if we need to
  if(headerRecv != PACKETHEADERSIZE)
    {
      if(readQueueLen >= PACKETHEADERSIZE)
	{
	  headerRaw = readQueue[readQueueBegin] << 8;
	  headerRaw |= readQueue[(readQueueBegin + 1) % readQueueSize];
	  readQueueBegin = (readQueueBegin + PACKETHEADERSIZE) % readQueueSize;
	  readQueueLen -= PACKETHEADERSIZE;
	  headerRecv = PACKETHEADERSIZE;
	  startTime = time(NULL);
	}
      else
	{
	  return 0;
	}
    }
  
  if(headerRaw == 0)
    {
      handleKeepAlive();
      return 0;
    }
  
  //If the read queue has enough data to make up the current packet
  if(readQueueLen >= headerRaw)
    {
      //LOG(DEBUG) << "Handling Packet. Header size: " << headerRaw << " readQueueLen: " << readQueueLen;
      handlePacket(packet);
      return 1;
    }

  if(difftime(time(NULL), startTime) >= TIMEOUT)
    {
      handleTimeout();
      return 0;
    }

  return 0;
}


void TCPPacketConn::writeData()
{
  //LOG(DEBUG) << "Writing to Socket";
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
  //LOG(DEBUG) << "Bytes written: " << status;

  writeQueueBegin = (writeQueueBegin + status) % writeQueueSize;
  writeQueueLen -= status;
}


void TCPPacketConn::insertData(const uint8_t *buffer, int len)
{
  //LOG(DEBUG) << "Inserting Data";
  std::lock_guard<std::mutex> lockGuard(queueMutex);
  if((writeQueueLen + len + PACKETHEADERSIZE) > writeQueueSize)
    {
      writeQueue = resizeQueue(writeQueue, &writeQueueSize, &writeQueueBegin, writeQueueLen, writeQueueLen + len + PACKETHEADERSIZE);
    }
  
  writeQueue[(writeQueueBegin + writeQueueLen)%writeQueueSize] = len >> 8;
  writeQueueLen++;
  writeQueue[(writeQueueBegin + writeQueueLen)%writeQueueSize] = len & 0xFF;
  writeQueueLen++;
  for(int i = 0; i < len; i++)
    {
      writeQueue[(writeQueueBegin + writeQueueLen) % writeQueueSize] = buffer[i];
      writeQueueLen++;
    }

  //LOG(DEBUG) << "Write Queue Length is now: " << writeQueueLen << " for fd " << fd;;
}
