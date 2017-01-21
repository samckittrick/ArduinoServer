/*
    Device Manager for Arduino Server

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

    Created by: Scott McKittrick, Jan. 20th 2017

*/

#include "DeviceManager.h"

int DeviceManager::addDevice(int devType, std::string devAddr)
{
  try
    {
      switch(devType)
	{
	case DEV_TYPE_SERIAL:
	  devList.push_back(new BasicSerialDevice(devAddr));
	  std::cout << "Adding Serial Device: " << devAddr << std::endl;
	  break;
	default:
	  return -1;
	}

      return 0;
    }
  catch(ConnectionException e)
    {
      std::cout << "Error Loading Device: " << e.what() << std::endl;
      return -1;
    }
}

const std::vector<BasicDevice*> DeviceManager::getDeviceList() const
{
  return devList;
}
