CC=g++
CFLAGS=-pthread -std=c++11
INCLUDE=-I../include/ -I../SerialPacketConn/include/ -I../CPPLogger/

all: DeviceManagerTest BasicSerialDeviceTest

DeviceManagerTest: DeviceManager.o DeviceManagerTest.cpp
	$(CC) $(CFLAGS) $(INCLUDE) ../build/* ../SerialPacketConn/build/* DeviceManagerTest.cpp -o $@

BasicSerialDeviceTest: BasicSerialDevice.o BasicSerialDeviceTest.cpp
	$(CC) $(CFLAGS) $(INCLUDE) ../build/* ../SerialPacketConn/build/* BasicSerialDeviceTest.cpp -o $@

.PHONY: DeviceManager.o
DeviceManager.o:
	$(MAKE) -C .. build/DeviceManager.o

.PHONY: BasicSerialDevice.o
BasicSerialDevice.o:
	$(MAKE) -C .. build/BasicSerialDevice.o

clean:
	rm -f BasicSerialDeviceTest
	rm -f DeviceManagerTest
	$(MAKE) -C .. clean
