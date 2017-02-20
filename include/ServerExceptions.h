/*
    Exception Classes for Arduino Server

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

    Created by: Scott McKittrick, Jan. 15th 2017

*/
#ifndef SERVEREXCEPTIONS_H
#define SERVEREXCEPTIONS_H
#include <exception>

struct ConnectionException : public std::exception {
  std::string message;
  ConnectionException(std::string m) {
    message = m;
  }

  const char * what() {
    return message.c_str();
  }
};

struct CommunicationException : public std::exception {
  std::string message;
  CommunicationException(std::string m){
    message = m;
  }

  const char * what() { 
    return message.c_str();
  }
};

struct IDNotFoundException : public std::exception {
  std::string message;
  IDNotFoundException(std::string m) {
    message = m;
  }

  const char * what() {
    return message.c_str();
  }
};

#endif
