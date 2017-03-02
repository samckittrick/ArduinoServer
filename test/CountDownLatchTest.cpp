#include <thread>
#include "CountDownLatch.h"
#include <iostream>
#include <unistd.h>
using namespace std;

void countDown(CountDownLatch&  latch)
{
  for(int i = 10; i > 0; i--)
    {
      sleep(1);
      cout << "T-Minus: " << i << endl;
      latch.countDown();
    }
}

int main()
{
  CountDownLatch latch(10);


  cout << "Starting count." << endl;
  std::thread threadObj(countDown, std::ref(latch));
  latch.await();
  cout << "Lift off!" << endl;
  return 0;
}
