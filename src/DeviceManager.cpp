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

DeviceManager::DeviceManager()
{
  LOG(INFO) << "Starting Device Manager";
  exitCondition = false;
  processThread = std::thread(&DeviceManager::processQueue, this);
}

//Add a device to the list. If there is an error connecting, return -1.
int DeviceManager::addDevice(int devType, std::string devAddr)
{
  std::lock_guard<std::mutex> lockGuard(devListMutex);
  try
    {
      switch(devType)
	{
	case DEV_TYPE_SERIAL:
	  devList.push_back(new BasicSerialDevice(devAddr));
	  LOG(INFO) << "Added Serial Device: " << devAddr;
	  break;
	default:
	  LOG(WARN) << "Unknown Device Type";
	  return -1;
	}

      return 0;
    }
  catch(ConnectionException e)
    {
      LOG(ERROR) << "Error Loading Device: " << e.what();
      return -1;
    }
}

const std::vector<BasicDevice*> DeviceManager::getDeviceList() const
{
  return devList;
}

//Find the device by it's id. Return null if the device doesn't exist.
BasicDevice* DeviceManager::getDeviceById(uint8_t id)
{
  std::lock_guard<std::mutex> lockGuard(devListMutex);
  for(std::vector<BasicDevice*>::iterator it = devList.begin(); it != devList.end(); it++)
    {
      if(id == (*it)->getDeviceId())
	{
	  return *it;
	}
    }
  return NULL;
}

BasicDevice* DeviceManager::getDeviceByAddress(uint8_t addr)
{
  std::lock_guard<std::mutex> lockGuard(devListMutex);
  //Check if the address has a device
  if(addr < 1 || addr > devList.size())
    {
      return NULL;
    }
  else
    {
      return devList.at(addr);
    }
}

//Add a request to the request queue
void DeviceManager::addRequest(const RequestObj& req)
{
  LOG(DEBUG) << "Adding Request";
  LOG(DEBUG) << req.toString();
  std::lock_guard<std::mutex> lockGuard(reqListMutex);
  reqQueue.push(req);
  hasRequest.notify_one();
}

void DeviceManager::processQueue()
{
  while(!exitCondition.load())
    {
      std::unique_lock<std::mutex> l(reqListMutex);
      if(reqQueue.empty())
	{
	  hasRequest.wait(l, [this](){return !reqQueue.empty() | exitCondition.load(); } );
	}

      //Do something with the request here.
      RequestObj req = reqQueue.front();
      reqQueue.pop();
      LOG(DEBUG) << "Processing Request";
      LOG(DEBUG) << req.toString();

    }
}

void DeviceManager::endThreads()
{

  //Kill the processQueue thread
  exitCondition = true;
  hasRequest.notify_one();
  processThread.join();

  //Kill all device threads
  std::lock_guard<std::mutex> lockGuard(devListMutex);
  for(std::vector<BasicDevice*>::iterator it = devList.begin(); it != devList.end(); it++)
    {
      (*it)->signalEnd();
    }
}
 
  
