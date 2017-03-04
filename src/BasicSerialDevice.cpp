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

BasicSerialDevice::BasicSerialDevice(std::vector<std::string> devParams)
{
  receiver = NULL;
  
  LOG(INFO) << "Initializing Serial Device.";
  conn.setDeviceName(devParams[0]);
  conn.setBaudRate(9600);
  //conn.setPacketReceiver(&packetReceiver);
  
  if(conn.connect() == -1 )
    throw ConnectionException("Error Connecting to Serial Device");
  //usleep(1000000);


  LOG(DEBUG) << "Getting information from device";
  //Lets find out information about this device
  struct command message;
  message.cmd = DEV_GET_INFO;
  struct command response;
  int result;
  for(int i = 0; i < 5; i++) //Retry Logic since arduino resets when serial device is opened
    {
      sendCommand(message);
  
      result = recvCommand(&response);
      if(result > -1)
	{
	  break;
	}
      else
	{
	  usleep(1000000);
	  LOG(WARN) << "Device didn't respond. Retrying...";
	}
    }

  if(result == -1)
    throw ConnectionException("Device did not respond");

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

  //Begin the read thread.
  LOG(DEBUG) << "Starting read thread";
  endCond = false;
  readThreadObj = std::thread(&BasicSerialDevice::readThread, this);
  
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
  uint8_t commandArr[message.data.size() + 4];
  commandArr[0] = message.cmd >> 8;
  commandArr[1] = message.cmd & 0x00FF;
  commandArr[2] = message.src;
  commandArr[3] = message.dst;
  for(int i = 0; i < message.data.size(); i++)
    {
      //Offset the data by 2 since we already inserted the command value
      commandArr[i+4] = message.data[i];
    }

  //Send the message
  conn.sendMessage(commandArr, message.data.size() + 4);
}
  
int BasicSerialDevice::recvCommand(struct command *rsp)
{
  //LOG(DEBUG) << "Listening for response";
  //Wait for a response, may need a timeout here.
  uint8_t responseArr[MAXDATALEN];
  int recvLen = conn.recvData(responseArr, MAXDATALEN, 2);
  
  //Make sure we got data back
  //We should at least get two bytes for the command response even if there's no data to go with it
  if(recvLen <= 1)
    {
      return -1;
    }

  //Parse the response
  rsp->cmd = responseArr[0] << 8;
  rsp->cmd += responseArr[1];
  rsp->src = responseArr[2];
  rsp->dst = responseArr[3];

  //If there is data, parse that too
  if(recvLen >4)
    {
      for(int i = 4; i < recvLen; i++)
	{
	  rsp->data.push_back(responseArr[i]);
	}
    }
  else
    {
      rsp->data.clear();
    }
  return 0;
}

//Call back for when a device sends a message back
void BasicSerialDevice::commandReceived(struct command message)
{
  LOG(DEBUG) << "Packet Received: " << message.cmd;
  if(receiver != NULL)
    {
      receiver(command2ReqObj(message));
    }
}

//Thread function for reading messages from serial device
void BasicSerialDevice::readThread()
{
  LOG(DEBUG) << "Read thread started";
  while(!endCond.load())
    {
      struct command rsp;
      int result = recvCommand(&rsp);
       if(result != -1)
	 commandReceived(rsp);
    }
}

//Used to end the read thread
void BasicSerialDevice::signalEnd()
{
  LOG(INFO) << "Ending thread..";
  endCond = true;
  readThreadObj.join();
}

struct BasicDevice::command BasicSerialDevice::reqObj2Command(const RequestObj& req)
{
  struct command cmd;
  cmd.cmd = req.getCommand();
  cmd.dst = req.getDest();
  cmd.src = req.getSrc();
  cmd.data = req.getData();
  return cmd;
}

 RequestObj BasicSerialDevice::command2ReqObj(struct command m)
{
  return RequestObj(m.cmd, m.dst, m.src, m.data);
}

void BasicSerialDevice::processRequest(const RequestObj& req)
{
  sendCommand(reqObj2Command(req));
}

void BasicSerialDevice::setRequestReceiver(std::function<RequestReceiver> r)
{
  receiver = r;
}


