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

#include <stdint.h>
#include <string>

#include "ServerExceptions.h"
#include "ProtocolConsts.h"
#include "LinuxSerialPacketConn.h"
class BasicSerialDevice {

 public:
  BasicSerialDevice(std::string devPath);
  ~BasicSerialDevice();
  const uint8_t getDeviceId() const;
  const uint8_t getDeviceType() const;
  const std::string getDeviceName() const;
  //const uint8_t* getStatus();
  

 private:
  LinuxSerialPacketConn conn;
  DevInfo devInfo;
  //void packetReceiver(const uint8_t *payload, int payloadLength);
  


};
