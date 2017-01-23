/*
   Basic Serial Device Class

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

    Created by: Scott McKittrick, Jan. 15th 2017

*/

#include "BasicSerialDevice.h"

BasicSerialDevice::BasicSerialDevice(std::string devPath)
{
  LOG(DEBUG) << "Initializing Serial Device.";
  conn.setDeviceName(devPath);
  conn.setBaudRate(9600);
  //conn.setPacketReceiver(&packetReceiver);
  
  if(conn.connect() == -1 )
    throw ConnectionException("Error Connecting to Serial Device");
  //usleep(1000000);


  LOG(DEBUG) << "Getting information from device";
  //Lets find out information about this device
  struct command message;
  message.cmd = DEV_GET_INFO;
  sendCommand(message);
  
  
  struct command response = recvCommand();
  LOG(DEBUG) << "Response received in constructor";
  //Read the response;
  if(response.cmd == DEV_GET_INFO_RSP)
    {
      devInfo.devid = response.data[0];
      devInfo.type = response.data[1];
      devInfo.devname = (char*)(&response.data[2]);
      LOG(DEBUG) << "Device Description: ID: " << devInfo.devid << " Type: " << devInfo.type << " Name: " << devInfo.devname;
    }
  else
    {
      LOG(ERROR) << "Device info not found or not valid";
      throw ConnectionException("Device is not correct type");
    }
  
}

BasicSerialDevice::~BasicSerialDevice()
{
  conn.disconnect();
}

const uint8_t BasicSerialDevice::getDeviceId() const
{
  return devInfo.devid;
}

const uint8_t BasicSerialDevice::getDeviceType() const
{
  return devInfo.type;
}

const std::string BasicSerialDevice::getDeviceName() const
{
  return devInfo.devname;
}

void BasicSerialDevice::sendCommand(struct BasicDevice::command message)
{
  LOG(DEBUG) << "Sending Message| Command:  " << message.cmd;
  uint8_t commandArr[message.data.size() + 2];
  commandArr[0] = message.cmd >> 8;
  commandArr[1] = message.cmd & 0x00FF;
  for(int i = 0; i < message.data.size(); i++)
    {
      //Offset the data by 2 since we already inserted the command value
      commandArr[i+2] = message.data[i];
    }

  //Send the message
  conn.sendMessage(commandArr, message.data.size() + 2);
}
  
struct BasicDevice::command BasicSerialDevice::recvCommand()
{
  LOG(DEBUG) << "Receiving response";
  //Wait for a response, may need a timeout here.
  uint8_t responseArr[MAXDATALEN];
  int recvLen = conn.recvData(responseArr, MAXDATALEN, 10);
  
  //Make sure we got data back
  //We should at least get two bytes for the command response even if there's no data to go with it
  if(recvLen <= 1)
    {
      LOG(ERROR) << "No Data received";
      throw CommunicationException("No Data Returned");
    }

  //Parse the response
  struct command rsp;
  rsp.cmd = responseArr[0] << 8;
  rsp.cmd += responseArr[1];

  //If there is data, parse that too
  if(recvLen >2)
    {
      for(int i = 2; i < recvLen; i++)
	{
	  rsp.data.push_back(responseArr[i]);
	}
    }
  else
    {
      rsp.data.clear();
    }
  return rsp;
}
