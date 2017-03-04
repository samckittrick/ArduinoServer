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
    
    Created by: Scott McKittrick, Jan. 11th 2017
    
*/

#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include "BasicDevice.h"
#include "BasicSerialDevice.h"
#include "ProtocolConsts.h"
#include "CPPLogger.h"
#include "RequestObj.h"

class DeviceManager
{
    public:
        DeviceManager();
        //Device Enumeration and management functions
        int addDevice(int devType, std::vector<std::string> devInfo);
	const std::vector<BasicDevice*> getDeviceList() const;
	BasicDevice* getDeviceById(uint8_t id);
	//Device addressing is based on a single byte address. Address 0 is devicemanager itself. 
	//Address 255 is the broadcast address, leaving 253 possible addresses for devices
	BasicDevice* getDeviceByAddress(uint8_t addr);

	//Request Management functions
	void addRequest(const RequestObj& req);
	void processQueue();
	static struct command ReqObj2Command(const RequestObj& req);
	static RequestObj Command2ReqObj(struct command m);
	static const std::vector<uint8_t> marshallDeviceList(const std::vector<BasicDevice*> list); 
	//Notify threads that an exit has been requested
	void endThreads();

	//Register a request callback
	void setRequestQueueListener(std::function<RequestReceiver> l);
    
    private:
        //Device list
	std::vector<BasicDevice*> devList;
	//Incoming Request Queue
	std::queue<RequestObj> reqQueue;
	
	//RequestQueueListener
	std::function<RequestReceiver> listener;
	
	//Thread Saftey and control
	std::mutex devListMutex;
	std::mutex reqListMutex;
	std::condition_variable hasRequest;
	std::atomic<bool> exitCondition;
	std::thread processThread;
};

#endif
