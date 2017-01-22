CC=g++
CFLAGS=-std=c++11
INCLUDE=-Iinclude/ -ISerialPacketConn/include/ -ICPPLogger/
BUILDDIR=build

all:

$(BUILDDIR)/DeviceManager.o: $(BUILDDIR)/BasicSerialDevice.o include/DeviceManager.h include/ProtocolConsts.h include/BasicDevice.h src/DeviceManager.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c src/DeviceManager.cpp -o $@

$(BUILDDIR)/BasicSerialDevice.o: LinuxSerialPacketConn.a src/BasicSerialDevice.cpp include/BasicDevice.h include/BasicSerialDevice.h include/ProtocolConsts.h 
	$(CC) $(CFLAGS) $(INCLUDE) -c src/BasicSerialDevice.cpp -o $@

.PHONY: LinuxSerialPacketConn.a
LinuxSerialPacketConn.a:
	$(MAKE) -C SerialPacketConn $@

clean: 
	rm $(BUILDDIR)/*
	$(MAKE) -C SerialPacketConn clean
