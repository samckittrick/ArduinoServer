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

TCPConn::~TCPConn() {}

TCPConn& operator=(TCPConn&& other) {
	receiver = other.receiver;
	authenticated = other.authenticated;
	conn = other.conn;
}

int getFd()
{
	return conn.getFd();
}

void writeData() {}

void readData()
{
	std::vector<uint8_t> packet;
	int status = conn.readData(packet);
	if(status == 0)
	{
		return;
	}
	else
	{
		LOG(DEBUG) << "Packet received in conn";
	}
}

void TCPConn::sendRequest(const RequestObj& req)
{
  LOG(DEBUG) << "Writing Packet";
}

void TCPConn::setRequestReceiver(RequestReceiver r)
{
  receiver = r;
}

bool TCPConn::isAuthenticated() const
{
  return authenticated;
}