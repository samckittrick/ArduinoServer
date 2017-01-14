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

#include "RequestObj.h"

RequestObj::RequestObj(SerialCommand_t cmd, size_t len, uint8_t *inData)
{
    command = cmd;
    dataLen = len;
    data = new uint8_t[len];
    
    //Copy the data by value
    for(int i = 0; i < len; i++)
    {
        data[i] = inData[i];
    }
}

RequestObj::~RequestObj()
{
    delete [] data;
}

SerialCommand_t RequestObj::getCommand() const
{
    return command;
}

size_t RequestObj::getDataLen() const
{
    return dataLen;
}

const uint8_t* RequestObj::getData() const
{
    return data;
}