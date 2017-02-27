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

    Created by: Scott McKittrick, Feb. 12th 2017
*/

#include "TCPConn.h"
TCPConn::TCPConn(int f) : conn(f) {}

TCPConn::TCPConn(const TCPConn& other) : conn(other.conn)
{
  //LOG(DEBUG) << "TCPConn COpy Constructor";
  authenticated = other.authenticated;
  receiver = other.receiver;
}
TCPConn::~TCPConn() {}

TCPConn& TCPConn::operator=(TCPConn&& other) {
	receiver = other.receiver;
	authenticated = other.authenticated;
	conn = std::move(other.conn);
}

int TCPConn::getFd() const
{
	return conn.getFd();
}

void TCPConn::writeData() 
{
  conn.writeData();
}

void TCPConn::readData()
{
	std::vector<uint8_t> packet;
	int status = conn.readData(packet);
	if(status == 0) //If we haven't gotten the full packet yet, read will return 0;
	{
		return;
	}
	else
	{
	  uint8_t packetType = packet.front();
	  packet.erase(packet.begin());
	  if(packetType == AUTHPACKETTYPE)
	    {
	      uint8_t *rsp;
	      try
		{
		  int rspLen = authenticator.authenticate(packet.data(), packet.size(), &rsp);
		  conn.insertData(rsp, rspLen);
		  free(rsp);
		}
	      catch(AuthenticationFailedException e)
		{
		  LOG(ERROR) << "Authentication Failed: " << e.what();
		}
	    }
	  else if((packetType == DATAPACKETTYPE) && (authenticator.getAuthStatus() == AUTHSTATE_AUTHENTICATED))
	    {
	      LOG(DEBUG) << "Data packet found";
	    }
	  else
	    {
	      LOG(WARN) << "Invalid or unauthorized packet found";
	    }
	}
}

void TCPConn::sendRequest(const RequestObj& req)
{
  //The only messages allowed in or out when we are unauthenticated are auth packets
  if(authenticator.getAuthStatus() != AUTHSTATE_AUTHENTICATED)
    {
      return;
    }

  //LOG(DEBUG) << "Writing Packet";
  int len = 4 + req.getData().size();
  uint8_t buffer[len];
  buffer[0] = DATAPACKETTYPE;
  buffer[1] = req.getDest();
  buffer[2] = req.getCommand() >> 8;
  buffer[3] = req.getCommand() & 0xFF;
 
  std::vector<uint8_t> data = req.getData();
  for(int i = 0; i < data.size(); i++)
    {
      buffer[i + 3] = data[i];
    }

  conn.insertData(buffer, len);
}

void TCPConn::setRequestReceiver(RequestReceiver r)
{
  receiver = r;
}

bool TCPConn::isAuthenticated() const
{
  return authenticated;
}

bool TCPConn::readyToWrite()
{
  return conn.readyToWrite();
}
