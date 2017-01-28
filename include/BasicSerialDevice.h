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

#ifndef BASICSERIALDEVICE_H
#define BASICSERIALDEVICE_H

#include <stdint.h>
#include <string>
#include <atomic>
#include <thread>
#include <unistd.h>
#include "BasicDevice.h"
#include "ServerExceptions.h"
#include "ProtocolConsts.h"
#include "LinuxSerialPacketConn.h"
#include "CPPLogger.h"
class BasicSerialDevice : public BasicDevice {

 public:
  BasicSerialDevice(std::string devPath);
  ~BasicSerialDevice();

  const uint8_t getDeviceId() const;
  const uint8_t getDeviceType() const;
  const std::string getDeviceName() const;
  
  void sendCommand(struct command message);
  int recvCommand(struct command *rsp);

  void commandReceived(struct command message);

  void processRequest(const RequestObj& req);
  void setRequestReceiver(requestReceiver r);

  static struct command reqObj2Command(const RequestObj& req);
  static RequestObj command2ReqObj(struct command m);

  void readThread();
  void signalEnd();

 private:
  LinuxSerialPacketConn conn;
  DevInfo devInfo;
  std::atomic<bool> endCond;
  std::thread readThreadObj;
  requestReceiver receiver; 

};

#endif
