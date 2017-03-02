/*
    CountDownLatch for main thread.
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
*/
#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include <atomic>
#include <mutex>
#include <condition_variable>

class CountDownLatch
{
 public:

  CountDownLatch(int n);

  void countDown();
  void countDown(int n);
  
  void await();

private:
int count;
std::condition_variable condVar;
std::mutex mutex;
};



#endif
