#include <iostream>
#include "CPPLogger.h"
#include "TCPManager.h"

using namespace std;

int main()
{
  CPPLogger::getLog().setLogLevel(DEBUG);

  TCPManager mgr(50001);
  mgr.init();

  std::string line;
  getline(cin, line);

  std::vector<uint8_t> d = { 0x45, 0x46, 0x50, 0x51 };
  RequestObj req(DEV_GET_INFO, 0x04, 0x00, d);
  mgr.addRequest(req);
  
  getline(cin, line);

  mgr.setExitCondition(true);
  
  return 0;
}
