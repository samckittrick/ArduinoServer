/*
  Arduino Server 
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
    Created by: Scott McKittrick, March 1st, 2017

`This server is intended to be an interface between various clients and multiple arduino devices.
*/

#include <unistd.h>
#include <string>
#include <iostream>
#include <functional>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include "CPPLogger.h"
#include "TCPManager.h"
#include "DeviceManager.h"
#include "RequestObj.h"
#include "Authenticator.h"
#include "SharedSecretScheme.h"
#include "ProtocolConsts.h"
#include "CountDownLatch.h"

int debugLevel = INFO;
bool foreground;
std::string configFilePath = "";
std::string logFilePath = "/tmp/ArduinoServer.log";
int tcpPort = 50001;
CountDownLatch *latch;

//Components
TCPManager *tcpManager;
DeviceManager *deviceManager;

void signalHandler(int signum)
{
  std::string sigString;
  switch(signum)
    {
    case SIGTERM:
      sigString = "SIGTERM";
      break;
    case SIGINT:
      sigString = "SIGINT";
      break;
    default:
      sigString = "UNKNOWN";
      break;
    }
  LOG(INFO) << "Received Signal: " << sigString;
  latch->countDown(2);
}

void registerAuthenticators()
{
  //this will read the configuration file and register the appropriate authenticators
  SharedSecretScheme::setSharedSecret("SECRET");
  SharedSecretSchemeFactory *SScheme = new SharedSecretSchemeFactory();
  Authenticator::registerAuthenticationScheme(SScheme->getTag(), SScheme);
}

void configureDevices(DeviceManager* dm)
{
  //This will read the configuration object and add the list of devices. 
  std::vector<std::string> devInfo;
  devInfo.push_back("/dev/ttyACM0");
  int result = dm->addDevice(DEV_TYPE_SERIAL, devInfo);
  if(result != 0)
    {
      LOG(ERROR) << "Failed to initialize device: " << devInfo[0];
    }
}
  

//Change this to return a pointer to a config object
void parseConfigurationFile(std::string filePath)
{
  if(filePath.empty())
    {
      throw ConfigurationException("No config path specified");
    }
}

void printHelp()
{
  std::cout << "Arduino Server\nServer for connecting clients to arduino devices" << std::endl;
  std::cout << "By Scott McKittrick";
  std::cout << "Options\n";
  std::cout << "\t -c Path to config file\n";
  std::cout << "\t -d Debug level\n";
  std::cout << "\t -f Run in foreground\n";
  std::cout << "\t -h Show this help\n";
  std::cout << "\t -l Path to log file\n";
}


int main(int argc, char **argv)
{  
  
  //Prepare to parse options lsit
  char const *optlist = "d:fc:l:h";
  extern char *optarg;
  extern int optind;

  int c;
  while((c = getopt(argc, argv, optlist)) != -1)
    {
      switch(c)
	{
	case 'd':
	  debugLevel = atoi(optarg);
	  break;
	case 'f':
	  foreground = true;
	  break;
	case 'c':
	  configFilePath = optarg;
	  break;
	case 'l':
	  logFilePath = optarg;
	  break;
	case 'h':
	case 'H':
	default:
	  printHelp();
	  return 0;
	  break;
	}
    }
  std::cout << "Debug Level: " << debugLevel << std::endl;

  if(configFilePath.empty())
    {
      std::cout << "Error: no configuration file provided" << std::endl;
      return 1;
    }

  //Configure the logger
  CPPLogger::getLog().setLogLevel(debugLevel);
  if(!foreground)
    {
      LOG(INFO) << "Switching to log file...";
      CPPLogger::getLog().setLogModeFile(logFilePath);
    }

  LOG(INFO) << "Logger Configured.";

  std::string tmpLogPath = logFilePath;
  parseConfigurationFile(configFilePath);

  if((logFilePath.compare(tmpLogPath) != 0) && !foreground)
    {
      CPPLogger::getLog().setLogModeFile(logFilePath);
    }

  if(!foreground)
    {
      //Demonize!!
      LOG(INFO) << "Preparing to Fork";
      pid_t pid, sid;
      pid = fork();
      
      if(pid < 0)
	{
	  LOG(FATAL) << "Failed to fork. Exiting.";
	  return 1;
	}

      if(pid > 0)
	{
	  LOG(INFO) << "Fork Successful. Exiting parent process.";
	  return 0;
	}
      
      //change the file mask
      umask(0);
      sid = setsid();
      if(sid < 0)
	{
	  LOG(FATAL) << "Failed to get sid";
	  return 1;
	}

      if((chdir("/")) < 0)
	{
	  LOG(FATAL) << "Failed to change directory";
	}

      //Close standard file descriptors
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);

      LOG(INFO) << "Successfully daemonized. New PID: " << pid;
    }

  //Register Authenticators based on config
  registerAuthenticators();

  //register signal handler here
  latch = new CountDownLatch(2);
  signal(SIGTERM, signalHandler);
  signal(SIGINT, signalHandler);

  LOG(INFO) << "Starting TCPManager...";
  tcpManager = new TCPManager(tcpPort);
  try
    {
      tcpManager->init();
    }
  catch(ConnectionException e)
    {
      LOG(FATAL) << "Failed to initialize TCP Socket. Dying...";
      return 1;
    }
  LOG(DEBUG) << "Starting Device Manager...";
  deviceManager = new DeviceManager();
  std::function<RequestReceiver> devRequestReceiver = std::bind(&DeviceManager::addRequest, deviceManager, std::placeholders::_1);
  tcpManager->setRequestQueueListener(devRequestReceiver);
  std::function<RequestReceiver> tcpRequestReceiver = std::bind(&TCPManager::addRequest, tcpManager, std::placeholders::_1);
  deviceManager->setRequestQueueListener(tcpRequestReceiver);

  //configure the devices to be managed
  configureDevices(deviceManager);
  
  latch->await();
  LOG(INFO) << "Exiting...Goodbye";
  return 0;
}
	  
  
