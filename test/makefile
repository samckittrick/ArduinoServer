CC=g++
export DEBUGSYM=-g
CFLAGS=-pthread -lcrypto -std=c++11 $(DEBUGSYM)
INCLUDE=-I../include/ -I../SerialPacketConn/include/ -I../CPPLogger/

all: DeviceManagerTest BasicSerialDeviceTest TCPManagerTest TCPPacketConnTest

AuthenticatorTest: Authenticator.o SharedSecretScheme.o AuthenticatorTest.cpp
	$(CC) $(CFLAGS) $(INCLUDE) ../build/* AuthenticatorTest.cpp -o $@

TCPPacketConnTest: TCPPacketConn.o TCPPacketConnTest.cpp
	$(CC) $(CFLAGS) $(INCLUDE) ../build/* TCPPacketConnTest.cpp -o $@

TCPManagerTest: TCPManager.o Authenticator.o SharedSecretScheme.o TCPManagerTest.cpp
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

.PHONY: Authenticator.o
Authenticator.o:
	$(MAKE) -C .. build/Authenticator.o

.PHONY: SharedSecretScheme.o
SharedSecretScheme.o:
	$(MAKE) -C .. build/SharedSecretScheme.o

clean:
	rm -f BasicSerialDeviceTest
	rm -f DeviceManagerTest
	rm -f TCPManagerTest
	rm -f TCPPacketConnTest
	rm -f AuthenticatorTest
	$(MAKE) -C .. clean
