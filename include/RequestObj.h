/*
    Request Object to be sent between server components

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
    
    Created by: Scott McKittrick, Jan. 11th 2017
    
*/

#ifndef REQUESTOBJ_H
#define REQUESTOBJ_H

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "ProtocolConsts.h"

//Request object to be sent between components of the server
//Request objects are immutable
class RequestObj 
{

 public:
 RequestObj(uint16_t c, uint8_t d, uint8_t s, std::vector<uint8_t> da) : command(c), dst(d), src(s), data(da) {}
  uint16_t getCommand() { return command;}
  uint8_t getDest() { return dst; }
  uint8_t getSrc() { return src; }
  const std::vector<uint8_t>& getData() { return data; }
  std::string toString() const { 
    std::stringstream s;
    s << "RequestObject - Command: " << command << " Dst: " << dst << " Src: " << src;
    return s.str();
  }
  

 private:

  uint16_t command;
  uint8_t dst;
  uint8_t src;
  std::vector<uint8_t> data;
};
//Interface for transfering request objects between components
class RequestQueue
{
 public:
  virtual void addRequest(const RequestObj& req) = 0;
};


#endif 
