CC=g++
CFLAGS=-pthread -std=c++11 $(DEBUGSYM) 
INCLUDE=-Iinclude/ -ISerialPacketConn/include/ -ICPPLogger/
BUILDDIR=build

all:

$(BUILDDIR)/TCPConn.o: include/TCPConn.h src/TCPConn.cpp include/RequestObj.h CPPLogger/CPPLogger.h $(BUILDDIR)/TCPPacketConn.o
	$(CC) $(CFLAGS) $(INCLUDE) -c src/TCPConn.cpp -o $@

$(BUILDDIR)/TCPPacketConn.o: include/TCPPacketConn.h src/TCPPacketConn.cpp CPPLogger/CPPLogger.h
	$(CC) $(CFLAGS) $(INCLUDE) -c src/TCPPacketConn.cpp -o $@

$(BUILDDIR)/TCPManager.o: include/TCPManager.h src/TCPManager.cpp include/RequestObj.h CPPLogger/CPPLogger.h include/TCPPacketConn.h $(BUILDDIR)/TCPPacketConn.o
	$(CC) $(CFLAGS) $(INCLUDE) -c src/TCPManager.cpp -o $@

$(BUILDDIR)/DeviceManager.o: $(BUILDDIR)/BasicSerialDevice.o include/DeviceManager.h include/ProtocolConsts.h include/BasicDevice.h src/DeviceManager.cpp include/RequestObj.h CPPLogger/CPPLogger.h
	$(CC) $(CFLAGS) $(INCLUDE) -c src/DeviceManager.cpp -o $@

$(BUILDDIR)/BasicSerialDevice.o: LinuxSerialPacketConn.a src/BasicSerialDevice.cpp include/BasicDevice.h include/BasicSerialDevice.h include/ProtocolConsts.h CPPLogger/CPPLogger.h include/RequestObj.h 
	$(CC) $(CFLAGS) $(INCLUDE) -c src/BasicSerialDevice.cpp -o $@

.PHONY: LinuxSerialPacketConn.a
LinuxSerialPacketConn.a:
	$(MAKE) -C SerialPacketConn $@

clean: 
	rm -f $(BUILDDIR)/*
	$(MAKE) -C SerialPacketConn clean
