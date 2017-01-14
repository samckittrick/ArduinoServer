/*
   Protocol Constants for Arduino Control Server

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
    
    Created by: Scott McKittrick, Jan. 8th 2017
    
*/
#ifndef PROTOCOL_CONSTANTS_H
#define PROTOCOL_CONSTANTS_H
typedef enum SERIAL_COMMAND
{
    //Identification Constants
    DEV_GET_INFO = 0x0001,
    DEV_GET_INFO_RSP = 0x0100,
    DEV_SET_INFO = 0x0002,
    DEV_SET_INFO_RSP = 0x0200,
        
    //RGB Lamp Constants
    LAMP_OFF = 0x0010,
    LAMP_OFF_RSP = 0x1000,
    LAMP_SOLID = 0x0011,
    LAMP_SOLID_RSP = 0x1100,
    LAMP_FADE = 0x0013,
    LAMP_FADE_RSP = 0x1300,
    LAMP_SOLID_CYCLE = 0x0014,
    LAMP_SOLID_CYCLE_RSP = 0x1400,
    LAMP_GET_STATUS = 0x0015,
    LAMP_GET_STATUS_RSP = 0x1500 
} SerialCommand_t;

enum SERIAL_DEV_TYPE
{
    DEV_TYPE_LAMP = 0x01
};

typedef struct Dev_Info {
    std::string devname;
    std::string devid;
    SERIAL_DEV_TYPE type;
} DevInfo;
#endif //#ifndef PROTOCOL_CONSTANTS_H