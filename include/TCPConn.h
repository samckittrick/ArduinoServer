/*
  TCPConn Class
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

    Created by: Scott McKittrick, Feb. 11th 2017
*/

//TCPConn class: provide request management for individual connections

#ifndef TCPCONN_H
#define TCPCONN_H

#include <vector>
#include "RequestObj.h"
#include "TCPPacketConn.h"
#include "ServerExceptions.h"
#include "CPPLogger.h"

#define AUTHPACKETTYPE 0x01
#define DATAPACKETTYPE 0x02

class TCPConn
{
 public:
  TCPConn(int f);
  ~TCPConn();

  void sendRequest(const RequestObj& req);
  void setRequestReceiver(RequestReceiver r);
  void handlePacket(const std::vector<uint8_t>& data);
  
  bool isAuthenticated() const;

  void readData();
  void writeData();
  int getFd() const;

 private:
  TCPPacketConn conn;
  RequestReceiver receiver;
  bool authenticated;
  
};  

#endif