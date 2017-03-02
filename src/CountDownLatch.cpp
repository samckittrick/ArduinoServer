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

#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int n)
{
  std::lock_guard<std::mutex> lockGuard(mutex);
  count = n;
}

void CountDownLatch::countDown()
{
  countDown(1);
}

void CountDownLatch::countDown(int n)
{
  std::lock_guard<std::mutex> lockGuard(mutex);
  count -= n;
  if(count < 1)
    {
      condVar.notify_all();
    }
}

void CountDownLatch::await()
{
  std::unique_lock<std::mutex> ulock(mutex);
  if(count >= 1)
    {
      condVar.wait(ulock, [this](){return count < 1;});
    }
}
      
