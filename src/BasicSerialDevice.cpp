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
  conn.setDeviceName(devPath);
  conn.setBaudRate(9600);
  //conn.setPacketReceiver(&packetReceiver);
  
  if(conn.connect() == -1 )
    throw ConnectionException("Error Connecting to Serial Device");
  //usleep(1000000);
  uint16_t command = DEV_GET_INFO;
  uint8_t commandArr[2];
  commandArr[0] = DEV_GET_INFO>>8;
  commandArr[1] = DEV_GET_INFO & 0x00FF;
  conn.sendMessage(commandArr, 2);
  uint8_t response[20];
  conn.recvData(response, 20);
  command = response[0] << 8;
  command += response[1];
  if(command == DEV_GET_INFO_RSP)
    {
      devInfo.devid = response[2];
      devInfo.type = response[3];
      devInfo.devname = (char*)(response + 4);
    }
  else
    {
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

