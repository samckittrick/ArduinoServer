CC=g++
CFLAGS=-std=c++11
INCLUDE=-Iinclude/ -ISerialPacketConn/include/
BUILDDIR=build

all:

$(BUILDDIR)/BasicSerialDevice.o: LinuxSerialPacketConn.a src/BasicSerialDevice.cpp include/BasicSerialDevice.h include/ProtocolConsts.h 
	$(CC) $(CFLAGS) $(INCLUDE) -c src/BasicSerialDevice.cpp -o $@

.PHONY: LinuxSerialPacketConn.a
LinuxSerialPacketConn.a:
	$(MAKE) -C SerialPacketConn $@

clean: 
	rm $(BUILDDIR)/*
	$(MAKE) -C SerialPacketConn clean
