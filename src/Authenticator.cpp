/*
    Authenticator Object for Negotiating authentication scheme and authenticating clients
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

    Created by: Scott McKittrick, Feb 19th 2017

*/
#include "Authenticator.h"

std::map<std::string, AuthenticationSchemeFactory*> Authenticator::factoryMap;

Authenticator::Authenticator()
{
  authScheme = NULL;
  authStatus = AUTHSTATE_UNAUTHENTICATED;
}

void Authenticator::registerAuthenticationScheme(std::string tag, AuthenticationSchemeFactory* factory)
{
  if(factoryMap.find(tag) == factoryMap.end())
    {
      LOG(INFO) << "Adding " << tag << " factory to list";
      factoryMap.insert(std::make_pair(tag, factory));
    }
  else
    {
      LOG(WARN) << "Factory " << tag << " already exists in list";
    }
}

AuthenticationScheme* Authenticator::getAuthenticationScheme(std::string tag)
{
  std::map<std::string, AuthenticationSchemeFactory*>::iterator it = factoryMap.find(tag);
  //LOG(DEBUG) << "Requested Scheme length: " << tag.length();
  if(it != factoryMap.end())
    {
      return it->second->getAuthenticationScheme();
    }
  else
    {
      throw AuthenticationSchemeNotFoundException("Authentication Scheme not found");
    }
}

int Authenticator::getAuthStatus()
{
  return authStatus;
}

int Authenticator::authenticate(uint8_t *data, int length, uint8_t **rsp)
{
  //LOG(DEBUG) << "Received auth packet";
  if(authStatus == AUTHSTATE_UNAUTHENTICATED) //If it's unauthenticated, we need to select a scheme
    {
      LOG(DEBUG) << "Currently unauthenticated";
      if(data[0] != TYPE_CLIENT_HELLO)
	{
	  throw AuthenticationFailedException("Invalid Auth Message");
	}

      std::string requestedScheme((char*)(data + 1), length - 1);
      LOG(INFO) << "Client requested Auth Scheme: " << requestedScheme;

      try
	{
	  authScheme = getAuthenticationScheme(requestedScheme);
	  *rsp = new uint8_t[requestedScheme.size() + 1];
	  (*rsp)[0] = TYPE_SERVER_HELLO;
	  for(int i = 0; i < requestedScheme.size(); i++)
	    {
	      (*rsp)[i + 1] = requestedScheme.at(i);
	    }
	  authStatus = AUTHSTATE_SCHEMESELECTED;
	  return requestedScheme.size() + 1;
	}
      catch(AuthenticationSchemeNotFoundException e)
	{
	  LOG(ERROR) << "Authentication Scheme not found";
	  *rsp = new uint8_t[2];
	  (*rsp)[0] = TYPE_SERVER_HELLO;
	  (*rsp)[1] = 0x00;
	  return 2;
	}
    }
  else if(authStatus == AUTHSTATE_SCHEMESELECTED) //If we have a scheme selected pass the data to the scheme
    {
      LOG(DEBUG) << "Scheme has already been selected";
      if(data[0] != TYPE_AUTH_REQ)
	{
	  throw AuthenticationFailedException("Invalid Auth Method");
	}

      uint8_t* rspData;
      int rspLen = authScheme->authenticate(data+1, length - 1, &rspData);
      if(authScheme->getAuthStatus()) //If it's a challenge response theme, we may not be authenticated immediately. 
	{
	  LOG(INFO) << "Client is Authenticated";
	  authStatus = AUTHSTATE_AUTHENTICATED;
	}

      //Package the response data and return it
      *rsp = new uint8_t[rspLen + 1];
      (*rsp)[0] = authScheme->getAuthStatus() ? TYPE_AUTH_RSP_SUCCESS : TYPE_AUTH_RSP_MOREINFO;
      for(int i = 0; i < rspLen; i++)
	{
	  (*rsp)[i+1] = rspData[i];
	}
      
      if(rspLen > 0)
	{
	  delete [] rspData;
	}
      
      return rspLen + 1;
    }
}



