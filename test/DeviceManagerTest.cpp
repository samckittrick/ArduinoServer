#include <iostream>
#include <vector>
#include "DeviceManager.h"
#include "BasicDevice.h"
#include "ProtocolConsts.h"
#include "CPPLogger.h"
#include "RequestObj.h"

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

  std::string line;
  getline(std::cin, line);

  std::vector<uint8_t> d = { 0x03, 0x04, 0x05, 0x06 };
  RequestObj req(LAMP_SOLID, 0x41, 0x42, d);
  RequestObj req2(LAMP_OFF, 0x42, 0x43, d);
  RequestObj req3(LAMP_FADE, 0x43, 0x44, d);
  mgr.addRequest(req);
  mgr.addRequest(req2);
  mgr.addRequest(req3);

  mgr.endThreads();

  
}
