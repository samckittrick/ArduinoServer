/*
    Shared Secret Authentication Scheme
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
    
    Created by: Scott McKittrick, Feb 20th 2017
    
*/
#ifndef SHAREDSECRETSCHEME_H
#define SHAREDSECRETSCHEME_H


#include <stdint.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include "AuthenticationScheme.h"
#include "CPPLogger.h"
#include "ServerExceptions.h"

#define IDLENGTH 4
#define TIMELENGTH 20
#define MAXAUTHOFFSET 120
#define TIMEFORMAT "%FT%T%Z"//%T%z"

class SharedSecretScheme : public AuthenticationScheme
{

 public:
  static void setSharedSecret(std::string s) { sharedSecret = s; }
  int authenticate(uint8_t *data, int dataLen,  uint8_t **rsp);
  bool getAuthStatus();

 private:
  static std::string sharedSecret;
  bool authStatus;

  int digest_sha256(const unsigned char *message, size_t message_len, unsigned char **digest, unsigned int *digest_len);
};

class SharedSecretSchemeFactory : public AuthenticationSchemeFactory
{
 public:
  std::string getTag() { return "SHARED_SECRET_SCHEME"; };
  AuthenticationScheme* getAuthenticationScheme() { return new SharedSecretScheme; }
  

};

#endif
