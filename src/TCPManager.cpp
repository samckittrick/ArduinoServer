/*
    TCPManager for Arduino Server
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
    
    Created by: Scott McKittrick, Jan. 31th 2017
    
*/

#include "TCPManager.h"

TCPManager::TCPManager(unsigned int p) : port(p)
{
  exitCondition = false;
  //Load certificates,etc here.

}
  
TCPManager::~TCPManager()
{
  //Close Connection here
}

void TCPManager::init()
{
  LOG(INFO) << "Initializing TCP Connection...";

  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  //Convert the port number to a character array
  char portString[10];
  sprintf(portString, "%u", port);

  //Get Address info
  status = getaddrinfo(NULL, portString, &hints, &servinfo);
  if(status != 0)
    {
      LOG(ERROR) << "Error getting address info: " << gai_strerror(status);
      throw ConnectionException("Error getting address info.");
    }

  struct addrinfo *p;
  //Search the returned list for a socket we can bind to.
  for(p = servinfo; p != NULL; p = p->ai_next)
    {
      int status;
      sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if(sockfd == -1)
	{
	  LOG(WARN) << "Error connecting to socket: " << strerror(errno);
	  continue;
	}

      status = bind(sockfd, p->ai_addr, p->ai_addrlen);
      if(status == -1)
	{
	  LOG(WARN) << "Error binding socket: " << strerror(errno);
	  continue;
	}
      //If we successfully bind, end the loop early. 
      break;
    }

  //If we managed to get through the entire list without binding throw an exception
  if(p == NULL)
    {
      LOG(ERROR) << "Failed to bind socket.";
      throw ConnectionException("Failed to bind socket.");
    }
      
  freeaddrinfo(servinfo);

  status = listen(sockfd, LISTENERBACKLOG);
  if(status == -1)
    {
      LOG(ERROR) << "Failed to listen to socket. Error " << strerror(errno);
      throw ConnectionException("Failed to listen to socket.");
    }

  //Start read thread
  socketListen();

}

void TCPManager::socketListen()
{
  fd_set readfds;
  fd_set writefds;
  struct timeval tv;
  int status;
  while(!exitCondition.load())
    {
      //add the listening socket fd to the set
      FD_SET(sockfd, &readfds);
      int highFd = sockfd;

      //Add any others
      if(connList.size() > 0)
	{
	  for(std::vector<TCPConn>::iterator it = connList.begin(); it != connList.end(); it++)
	    {
	      FD_SET(it->getFd(), &readfds);
	      
	      //Only set the write fd if there is some data to write.
	      //if you set it all the time, select will always return immediately. 
	      //FD_SET(it->fd, &writefds);
	      if(it->getFd() > highFd)
		highFd = it->getFd();
	    }
	}

      //set timeout
      tv.tv_sec = 5;

      //Select the sockets
      status = select(highFd + 1, &readfds, &writefds, NULL, &tv);
      if(status == -1)
	{
	  LOG(ERROR) << "Error selecting socket: " << strerror(errno);
	}
      else if(status == 0)
	{
	  LOG(DEBUG) << "No data read, reselecting...";
	}
      else
	{
	  //If the listening socket is ready to read, there must be an incomming connection.
	  if(FD_ISSET(sockfd, &readfds))
	    {
	      struct sockaddr_storage their_addr;
	      socklen_t addr_size = sizeof their_addr;
	      int new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
	      if(new_fd == -1)
		{
		  LOG(ERROR) << "Error accepting connection: " << strerror(errno);
		}
	      else
		{
		  connList.emplace_back(new_fd);
		  LOG(INFO) << "Accepted connection from: " << "write function to get string. " << "Assigned local address: " << new_fd;
		}
	    }

	  //If there are connected sockets, check them too
	  if(connList.size() > 0)
	    {
	      for(std::vector<TCPConn>::iterator it = connList.begin(); it != connList.end(); it++)
		{
		  if(FD_ISSET(it->getFd(), &readfds))
		    {
		      LOG(DEBUG) << "Reading from fd: " << it->getFd();
		      //Do some reading maake sure read doesn;t block
		      it->readData();
		    }
		  
		  if(FD_ISSET(it->getFd(), &writefds))
		    {
		      LOG(DEBUG) << "Writing to fd: " << it->getFd();
		      //do some writing
		    }
		}
	    }
	}
    }
}
  

void TCPManager::setRequestQueueListener(RequestReceiver l)
{
  listener = l;
}


