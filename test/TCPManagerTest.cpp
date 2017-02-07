#include <iostream>
#include "CPPLogger.h"
#include "TCPManager.h"

int main()
{
  CPPLogger::getLog().setLogLevel(DEBUG);

  TCPManager mgr(50001);
  mgr.init();
  
  return 0;
}
