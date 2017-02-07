CC=g++
export DEBUGSYM=-g
CFLAGS=-pthread -std=c++11 $(DEBUGSYM)
INCLUDE=-I../include/ -I../SerialPacketConn/include/ -I../CPPLogger/

all: DeviceManagerTest BasicSerialDeviceTest TCPManagerTest TCPPacketConnTest

TCPPacketConnTest: TCPPacketConn.o TCPPacketConnTest.cpp
	$(CC) $(CFLAGS) $(INCLUDE) ../build/* TCPPacketConnTest.cpp -o $@

TCPManagerTest: TCPManager.o TCPManagerTest.cpp
	$(CC) $(CFLAGS) $(INCLUDE) ../build/* TCPManagerTest.cpp -o $@

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

.PHONY: TCPManager.o
TCPManager.o:
	$(MAKE) -C .. build/TCPManager.o

.PHONY: TCPPacketConn.o
TCPPacketConn.o:
	$(MAKE) -C .. build/TCPPacketConn.o

clean:
	rm -f BasicSerialDeviceTest
	rm -f DeviceManagerTest
	rm -f TCPManagerTest
	$(MAKE) -C .. clean
