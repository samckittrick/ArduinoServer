#include <iostream>
#include <vector>
#include "DeviceManager.h"
#include "BasicDevice.h"
#include "ProtocolConsts.h"
#include "CPPLogger.h"

int main()
{
  CPPLogger::getLog().setLogLevel(DEBUG);
  DeviceManager mgr;
  mgr.addDevice(DEV_TYPE_SERIAL, "/dev/ttyACM0");
  std::vector<BasicDevice*> list = mgr.getDeviceList();
  for(std::vector<BasicDevice*>::iterator it = list.begin(); it != list.end(); it++)
    {
      std::cout << "Device " << (*it)->getDeviceName() << " found.\n";
    }

  BasicDevice *dev = mgr.getDeviceById(0x45);
  std::cout << "Device by id 0x45 named " << dev->getDeviceName() << " found. \n";
}
