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

class DeviceManager
{
    public:
        DeviceManager();
        size_t getDeviceList(DevInfo devList, size_t size) const;
        int sendMessage(const RequestObj message);
        int sendBroadcast(const RequestObj message);    
    
    private:
        //Device info to FD mapping
        struct Device {
            DevInfo devInfo;
            int fileDescriptor;
        };
        struct Device *deviceList;
        size_t numDevices;
        
        //List of request objects waiting to be sent.
        RequestObj *inputQueue; //Turn this into a queue object?
    
        //Create reference to tcp controller for sending messages over the network.
        int getFileDescriptorById(std::string id);
        int getFileDescriptorByName(std::string name);
        
}