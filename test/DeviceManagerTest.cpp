#include <iostream>
#include <vector>
#include "DeviceManager.h"
#include "BasicDevice.h"
#include "ProtocolConsts.h"
#include "CPPLogger.h"
#include "RequestObj.h"

void receiver(const RequestObj& rsp)
{
  LOG(DEBUG) << "Received Response: " << rsp.toString();
}

int main()
{
  CPPLogger::getLog().setLogLevel(DEBUG);
  DeviceManager mgr;
  mgr.setRequestQueueListener(&receiver);
  std::vector<std::string> devInfo;
  devInfo.push_back("/dev/ttyACM0");
  mgr.addDevice(DEV_TYPE_SERIAL, devInfo);
  std::vector<BasicDevice*> list = mgr.getDeviceList();
  for(std::vector<BasicDevice*>::iterator it = list.begin(); it != list.end(); it++)
    {
      LOG(DEBUG) << "Device " << (*it)->getDeviceName() << " found.\n";
    }

  BasicDevice *dev = mgr.getDeviceById(0x45);
  LOG(DEBUG) << "Device by id 0x45 named " << dev->getDeviceName() << " found. \n";

  std::string line;
  getline(std::cin, line);

  std::vector<uint8_t> d = { 0x03, 0x04, 0x05, 0x06 };
  RequestObj req(DEV_GET_INFO, 0x00, 0x01, d);
  mgr.addRequest(req);

  getline(std::cin, line);

  RequestObj req2(LAMP_OFF, 0x01, 0x01, d);
  mgr.addRequest(req2);

  getline(std::cin, line);

  mgr.endThreads();

  
}
