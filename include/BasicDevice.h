/*
    Basic Device Class. Abstract interface representing Devices that can be commanded. 

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

#ifndef BASICDEVICE_H
#define BASICDEVICE_H

#include <stdint.h>
#include <string>
#include <vector>
#include "RequestObj.h"

class BasicDevice {

 public:
  //Command Struct
  struct command {
    uint16_t cmd;
    uint8_t src;
    uint8_t dst;
    std::vector<uint8_t> data;
  };

  virtual const uint8_t getDeviceId() const = 0;
  virtual const uint8_t getDeviceType() const = 0;
  virtual const std::string getDeviceName() const = 0;

  virtual void processRequest(const RequestObj& req) = 0;
  virtual void setRequestReceiver(RequestReceiver r) = 0;

  virtual void readThread() = 0;
  virtual void signalEnd() = 0;
};


#endif
