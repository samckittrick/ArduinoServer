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
#include "ProtocolConsts.h"

//Request object to be sent between components of the server
//Request objects are immutable
class RequestObj 
{
  public:
    RequestObj(SerialCommand_t cmd, size_t datalen, uint8_t *data);
    ~RequestObj();
    SerialCommand_t getCommand() const;
    size_t getDataLen() const;
    const uint8_t* getData() const;
    
    private:
        std::string devId;
        SerialCommand_t command;
        uint8_t *data;
        size_t dataLen;
};


#endif //#ifndef REQUESTOBJ_H
