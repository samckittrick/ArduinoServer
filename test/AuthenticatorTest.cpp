#include <string>
#include <stdint.h>
#include <algorithm>

#include "CPPLogger.h"
#include "Authenticator.h"
#include "SharedSecretScheme.h"

char convertHextoBinary(char c)
{
  const char quads[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
  if (c >= '0' && c <= '9') return quads[     c - '0'];
  if (c >= 'A' && c <= 'F') return quads[10 + c - 'A'];
  if (c >= 'a' && c <= 'f') return quads[10 + c - 'a'];
  return -1;
}

int main()
{
  CPPLogger::getLog().setLogLevel(DEBUG);
  Authenticator auth;
  SharedSecretScheme::setSharedSecret("SECRET");
  SharedSecretSchemeFactory* SScheme = new SharedSecretSchemeFactory();
  Authenticator::registerAuthenticationScheme(SScheme->getTag(), SScheme);
  

  //Create client hello 
  std::string tag = "SHARED_SECRET_SCHEME";
  uint8_t clientHello[tag.size() + 1];
  clientHello[0] = TYPE_CLIENT_HELLO;
  for(int i = 0; i < tag.size(); i++)
    {
      clientHello[i+1] = tag.at(i);
    }

  uint8_t* rsp;
  
  auth.authenticate(clientHello, tag.size() + 1, &rsp);

  delete [] rsp;

  int authRequestLen = 1 + 32 + 4 + 20;
  uint8_t authRequest[authRequestLen];
  memset(authRequest, 0, sizeof authRequest);
  authRequest[0] = TYPE_AUTH_REQ;
  
  std::string hash = "4FEB63FD427554739D208B1D57C44EC3687EB40671810B974BAA5FD248F5FD94";
  int authLoc = 1;
  for(int i = 0; i < hash.length(); i+=2)
    {
      authRequest[authLoc] = (convertHextoBinary(hash.at(i))<<4) | convertHextoBinary(hash.at(i+1));
      authLoc++;
    }
  authRequest[36] = 1;
  std::string time = "2017-02-27T06:59:30Z";
  //LOG(DEBUG) << "Presend length: " << time.length();
  //std::cout << time.c_str() << std::endl; 
  for(int i = 0; i < time.size(); i++)
    {
      authRequest[37+i] = time.at(i);
    }

  try
    {
      auth.authenticate(authRequest, authRequestLen, &rsp);
    }
  catch(AuthenticationFailedException e)
    {
      LOG(ERROR) << "Error: " << e.what();
    }
  
  
}
