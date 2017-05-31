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
    
    Created by: Scott McKittrick, Jan. 20th 2017
    
*/

#include "SharedSecretScheme.h"
#include <iostream>

std::string SharedSecretScheme::sharedSecret;

int SharedSecretScheme::authenticate(uint8_t *data, int dataLen, uint8_t **rsp)
{
  LOG(INFO) << "Authenticating client with shared secret";

  //Make sure the packet is long enough to have the full request
  if(dataLen < (SHA256_DIGEST_LENGTH + IDLENGTH + TIMELENGTH))
    {
      LOG(WARN) << "Auth message too short";
      throw AuthenticationFailedException("Auth message too short");
    }
  
  //Parse the packet
  uint8_t recvDigest[SHA256_DIGEST_LENGTH];
  
  int id;
  memmove(recvDigest, data,  SHA256_DIGEST_LENGTH);
  /*  LOG(DEBUG) << "Received Hash: ";
  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
      printf("%02X", recvDigest[i]);
    }
    printf("\n");*/


  //Parse the id
  int idOffset = SHA256_DIGEST_LENGTH;
  id = data[idOffset] << 24;
  id |= data[idOffset+1] << 16;
  id |= data[idOffset+2] << 8;
  id |= data[idOffset+3];

  LOG(DEBUG) << "ID is: " << id;
  

  //Parse the time
  std::string timestr((char*)(data + idOffset + IDLENGTH), TIMELENGTH);
  //LOG(DEBUG) << "Preparse time string is:" << timestr;
  struct tm recvTime;
  char* result = strptime(timestr.c_str(), TIMEFORMAT, &recvTime);
  if(result == nullptr)
    {
      throw AuthenticationFailedException("Invalid Time format");
    }

  char buffer[128];
  strftime(buffer, 128, TIMEFORMAT, &recvTime);
  LOG(DEBUG) << "Received Time is: " << buffer;

  //Parse the received time
  //We need to temporarily set the timezone env variable to utc before mktime will parse utc
  char* tz;
  tz = getenv("TZ");
  setenv("TZ", "", 1);
  tzset();
  time_t recvTimeUTC = mktime(&recvTime);
  if(tz)
    setenv("TZ", tz, 1);
  else
    unsetenv("TZ");
  tzset();
  //LOG(DEBUG) << "Received Time as PArsed:" << asctime(gmtime(&recvTimeUTC));
  
  //Calculate current time in UTC
  time_t currTimeLocal;
  time(&currTimeLocal);
  time_t currTimeUTC = currTimeLocal;
  LOG(DEBUG) << "Current UTC: " << asctime(gmtime(&currTimeUTC));

  //Compare time
  double diff = difftime(currTimeUTC, recvTimeUTC);
  LOG(DEBUG) << "Difference in seconds: " << diff;

  if(abs(diff) > MAXAUTHOFFSET)
    {
      LOG(ERROR) << "Timestamp too far off";
      //throw AuthenticationFailedException("Timestamp too far off.");
    }

  //Check that we have a shared secret
  if(sharedSecret.length() < 1)
    {
      throw AuthenticationFailedException("No shared secret set");
    }
  
  //Calculate sha256 hash
  unsigned char *digest;
  unsigned int digest_len;
  unsigned char message[sharedSecret.length() + IDLENGTH + TIMELENGTH];

  //String to be hashed is a concatenation of secret + id + timestamp
  for(int i = 0; i < sharedSecret.length(); i++)
    {
      message[i] = sharedSecret.at(i);
    }

  char idStr[10];
  int idStrLen = sprintf(idStr, "%d", id);
  //printf("ID String: %s\n", idStr);
  for(int i = 0; i < idStrLen; i++)
    {
      message[sharedSecret.length() + i] = idStr[i];
    }
  
  for(int i = 0; i < TIMELENGTH; i++)
    {
      message[i+sharedSecret.length() + idStrLen] = data[idOffset + IDLENGTH + i];
    }

  int dgstResult = digest_sha256(message, sharedSecret.length() + IDLENGTH + TIMELENGTH, &digest, &digest_len);
  if(dgstResult == -1)
    {
      throw AuthenticationFailedException("Failed to generate digest");
    }

  /*for(int i = 0; i < digest_len; i++)
    {
      printf("%02X", digest[i]);
    }
    printf("\n");*/

  //Compare hashes
  int compareResult = memcmp(recvDigest, digest, SHA256_DIGEST_LENGTH);
  
  if(compareResult != 0)
    {
      throw AuthenticationFailedException("Hash mismatch");
    }

  OPENSSL_free(digest);
  
  //If we have gotten this far, the authentication succeeded.
  LOG(INFO) << "Auth Succeeded";
  authStatus = true;
  return 0;
}

bool SharedSecretScheme::getAuthStatus()
{
  return authStatus;
}

int SharedSecretScheme::digest_sha256(const unsigned char *message, size_t message_len, unsigned char **digest, unsigned int *digest_len)
{
  EVP_MD_CTX *mdctx;

  mdctx = EVP_MD_CTX_create();
  if(mdctx == NULL)
    {
      LOG(ERROR) << "Failed to create digest context";
      return -1;
    }

  int result = EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);
  if(result != 1)
    {
      LOG(ERROR) << "Failed to initialize the digest context";
      return -1;
    }

  result  = EVP_DigestUpdate(mdctx, message, message_len);
  if(result != 1)
    {
      LOG(ERROR) << "Failed to insert data into digest";
      return -1;
    }

  *digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()));
  if(*digest == NULL)
    {
      LOG(ERROR) << "Failed to allocate digest memory";
      return -1;
    }

  result = EVP_DigestFinal_ex(mdctx, *digest, digest_len);
  if(result != 1)
    {
      LOG(ERROR) << "Failed to finalize digest";
      return -1;
    }

  EVP_MD_CTX_destroy(mdctx);
}
