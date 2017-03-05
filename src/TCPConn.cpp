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
  conn.readData();
  std::vector<uint8_t> packet;	
	//Process any packets that have come in
  while(conn.processData(packet))
    {
      uint8_t packetType = packet.front();
      packet.erase(packet.begin());
      if(packetType == AUTHPACKETTYPE)
	{
	  uint8_t *rsp;
	  try
	    {
	      int rspLen = authenticator.authenticate(packet.data(), packet.size(), &rsp);
	      uint8_t rspFinal[rspLen + 1];
	      rspFinal[0] = AUTHPACKETTYPE;
	      for(int i = 0 ; i < rspLen; i++)
		rspFinal[i+1] = rsp[i];
	      conn.insertData(rspFinal, rspLen + 1);
	      free(rsp);
	    }
	  catch(AuthenticationFailedException e)
	    {
	      LOG(ERROR) << "Authentication Failed: " << e.what();
	      throw ConnectionException("Authentication Failed");
	    }
	}
      else if((packetType == DATAPACKETTYPE) && (authenticator.getAuthStatus() == AUTHSTATE_AUTHENTICATED))
	{
	  try
	    {
	      RequestObj req = packageDataPacket(packet);
	      if(receiver != NULL)
		{
		  receiver(req);
		}
	    }
	  catch(CommunicationException e)
	    {
	      LOG(ERROR) << "Invalid data packet received";
	    }
	}
      else
	{
	  LOG(WARN) << "Invalid or unauthorized packet found";
	}
      packet.clear();
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
  buffer[1] = req.getSrc();
  buffer[2] = req.getCommand() >> 8;
  buffer[3] = req.getCommand() & 0xFF;
 
  std::vector<uint8_t> data = req.getData();
  for(int i = 0; i < data.size(); i++)
    {
      buffer[i + 4] = data[i];
    }

  conn.insertData(buffer, len);
}

void TCPConn::setRequestReceiver(std::function<RequestReceiver> r)
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

RequestObj TCPConn::packageDataPacket(const std::vector<uint8_t>& data)
{
  //If we don't at least get the device and command
  if(data.size() < 3)
    {
      LOG(ERROR) << "Invalid Packet Size";
      throw CommunicationException("Invalid Packet Size");
    }

  uint8_t dev = data.at(0);
  uint16_t cmd = (data.at(1) << 8) | data.at(2);

  std::vector<uint8_t> d(data.begin() + 3, data.end());

  RequestObj req(cmd, dev, getFd(), d);
  LOG(DEBUG) << "Request object packaged: " << req.toString();
  return req;
}
