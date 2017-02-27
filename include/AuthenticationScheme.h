/*
    Interfaces for Authentication Scheme and Authentication Scheme factory
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
    
    Created by: Scott McKittrick, Feb. 20th 2017
    
*/
#ifndef AUTHENTICATIONSCHEME_H
#define AUTHENTICATIONSCHEME_H

#include <stdint.h>
#include <string>

class AuthenticationScheme 
{
 public:
  virtual int authenticate(uint8_t *data, int dataLen, uint8_t **rsp) = 0;
  virtual bool getAuthStatus() = 0;
};

class AuthenticationSchemeFactory
{
 public:
  virtual std::string getTag() = 0;
  virtual AuthenticationScheme* getAuthenticationScheme() = 0;
  
};

#endif
