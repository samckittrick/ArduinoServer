#include <iostream>
#include <string>
#include "BasicSerialDevice.h"
#include "CPPLogger.h"
#include "RequestObj.h"


using namespace std;

void receiver(RequestObj req)
{
  LOG(DEBUG) << "Receiver called: " << req.toString();
}


int main()
{
  CPPLogger::getLog().setLogLevel(DEBUG);
  try
    {
      BasicSerialDevice dev("/dev/ttyACM0");
      dev.setRequestReceiver(&receiver);
      cout << "Connected\n";

      printf("Device Id: %02X\n", dev.getDeviceId());
      printf("Device Type %02X\n", dev.getDeviceType());
      cout << "Device Name: " << dev.getDeviceName() << endl;
      
      std::vector<uint8_t> d = { 0x01, 0x02, 0x03, 0x04 };
      RequestObj req(LAMP_SOLID, 0x41, 0x42, d);
      dev.processRequest(req);
      std::string line;
      getline(std::cin, line);

      dev.signalEnd();

    }
  catch(ConnectionException e)
    {
      cout << "Connection Exception caught:";
      cout << e.what() << endl;
      return 1;
    }

  return 0;
}






  
