CC=g++
CFLAGS=-std=c++11
INCLUDE=-I../include/ -I../SerialPacketConn/include/

BasicSerialDeviceTest: BasicSerialDevice.o BasicSerialDeviceTest.cpp
	$(CC) $(CFLAGS) $(INCLUDE) ../build/* ../SerialPacketConn/build/* BasicSerialDeviceTest.cpp -o $@

.PHONY: BasicSerialDevice.o
BasicSerialDevice.o:
	$(MAKE) -C .. build/BasicSerialDevice.o

clean:
	rm BasicSerialDeviceTest
	$(MAKE) -C .. clean
