
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
    
    Created by: Scott McKittrick, Jan. 29th 2017
    
*/

#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include <vector>
#include <queue>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <atomic>
#include <unistd.h>
#include "RequestObj.h"
#include "CPPLogger.h"
#include "ServerExceptions.h"
#include "TCPConn.h"

#define LISTENERBACKLOG 10

class TCPManager
{
 public:
  TCPManager(unsigned int p);
  ~TCPManager();

  void init();
  
  //Thread for listening on designated port
  void socketListen();

  //Register a request callback
  void setRequestQueueListener(RequestReceiver l);
  void addRequest(const RequestObj& req);

  void setExitCondition(bool cond);

 private:
  unsigned int port;
  int sockfd;
  std::atomic<bool> exitCondition;

  //Connection Object list
  std::vector<TCPConn> connList;
  TCPConn& getConnById(int id);
  
  //Request Queue callback
  RequestReceiver listener;

  std::thread readThread;

};

#endif
