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

  mgr.setExitCondition(true);
  
  return 0;
}
