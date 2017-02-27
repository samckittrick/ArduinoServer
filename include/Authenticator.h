/*
    Authenticator object for negotiating authentication scheme and authenticating clients
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

#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <map>
#include <string>
#include <stdint.h>
#include "AuthenticationScheme.h"
#include "CPPLogger.h"
#include "ServerExceptions.h"

#define AUTHSTATE_UNAUTHENTICATED 0
#define AUTHSTATE_SCHEMESELECTED 1 
#define AUTHSTATE_AUTHENTICATED 2

#define TYPE_CLIENT_HELLO 1
#define TYPE_SERVER_HELLO 2
#define TYPE_AUTH_REQ 3
#define TYPE_AUTH_RSP 4

class Authenticator
{
 public:
  Authenticator();

  //Function for authenticating a connection
  //data - authentication data from client
  //rsp - response data from server. Allows for multistep authentication
  //return - length of response
  int authenticate(uint8_t *data, int length, uint8_t **rsp);
  
  //Get the auth status
  int getAuthStatus();

  //For authentication scheme setup
  static void registerAuthenticationScheme(std::string tag, AuthenticationSchemeFactory* factory);
  
  
  
 private:
  static std::map<std::string, AuthenticationSchemeFactory*> factoryMap;
  AuthenticationScheme* authScheme;
  int authStatus;

  static AuthenticationScheme* getAuthenticationScheme(std::string tag);

};

#endif
  




















