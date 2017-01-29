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
  LOG(DEBUG) << "Adding Device: " << devAddr;
  std::lock_guard<std::mutex> lockGuard(devListMutex);
  try
    {
      BasicDevice *dev;
      switch(devType)
	{
	case DEV_TYPE_SERIAL:
	  dev = new BasicSerialDevice(devAddr);
	  devList.push_back(dev);
	  dev->setRequestReceiver(listener);
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
      //Need to check to see if the exit condition is true
      if(exitCondition.load())
	break;

      //Do something with the request here.
      RequestObj req = reqQueue.front();
      reqQueue.pop();

      LOG(INFO) << "Request from tcp addr " << (int)req.getSrc() << " to " << (int)req.getDest();
      //If this request was meant for DeviceManager
      if(req.getDest() == 0)
	{
	  if(req.getCommand() == DEV_GET_INFO)
	    {
	      std::vector<uint8_t> data = marshallDeviceList(getDeviceList());
	      RequestObj rsp(DEV_GET_INFO_RSP, req.getSrc(), 0, data);
	      if(listener != NULL)
		{
		  listener(rsp);
		}
	    }
	}
      else
	{
	  if(req.getDest() <= devList.size() || req.getDest() < 0)
	    {
	      devList.at(req.getDest() - 1)->processRequest(req);
	    }
	  else
	    {
	      LOG(INFO) << "Invalid Device address(" << (int)req.getDest() << ") requested from address " << (int)req.getSrc();
	    }
	}
	  //LOG(DEBUG) << "Processing Request";
	  // LOG(DEBUG) << req.toString();

    }
}

void DeviceManager::endThreads()
{

  //Kill the processQueue thread
  exitCondition = true;
  //We need to send the notify one to break the processQueue thread out of it's wait.
  //This is because there's no way to cancel a thread unless the thread agrees to it.

  hasRequest.notify_one();
  processThread.join();

  //Kill all device threads
  std::lock_guard<std::mutex> lockGuard(devListMutex);
  for(std::vector<BasicDevice*>::iterator it = devList.begin(); it != devList.end(); it++)
    {
      (*it)->signalEnd();
    }
}
 
const std::vector<uint8_t> DeviceManager::marshallDeviceList(const std::vector<BasicDevice*> list)
{
  int devEntrySize = 131;
  int nameStringLen = 128;
  std::vector<uint8_t> data;
  //First byte is the number of objects
  LOG(DEBUG) << "List size: " << list.size();
  data.push_back(list.size());
  
  for(int i = 0; i < list.size(); i++)
    {
      BasicDevice *dev = list.at(i);
      //Add the address of the device
      data.push_back(i & 0xFF);
      data.push_back(dev->getDeviceType());
      data.push_back(dev->getDeviceId());
      std::string devname = dev->getDeviceName();
      for(int j = 0; j < nameStringLen; j++)
	{
	  if(j < devname.size())
	    {
	      data.push_back(devname.at(j));
	    }
	  else
	    {
	      data.push_back(0x00);
	    }
	}
    }


  std::stringstream ss;
  ss<<std::hex;
  for(std::vector<uint8_t>::iterator it = data.begin(); it != data.end(); it++)
    {
      ss << (int)(*it);
    }
  LOG(DEBUG) << ss.str();

  return data;
}
      
void DeviceManager::setRequestQueueListener(RequestReceiver l)
{
  listener = l;
}
