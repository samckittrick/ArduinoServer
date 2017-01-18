#include <iostream>
#include "BasicSerialDevice.h"

using namespace std;

int main()
{
  try
    {
      BasicSerialDevice dev("/dev/ttyACM0");
      cout << "Connected\n";

      printf("Device Id: %02X\n", dev.getDeviceId());
      printf("Device Type %02X\n", dev.getDeviceType());
      cout << "Device Name: " << dev.getDeviceName() << endl;
    }
  catch(ConnectionException e)
    {
      cout << "Connection Exception caught:";
      cout << e.what() << endl;
      return 1;
    }

  return 0;
}
  
