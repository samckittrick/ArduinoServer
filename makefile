CC=g++
CFLAGS=-pthread -lcrypto -std=c++11 $(DEBUGSYM) 
INCLUDE=-Iinclude/ -ISerialPacketConn/include/ -ICPPLogger/
BUILDDIR=build
BINDIR=bin

all: $(BINDIR)/ArduinoServer

$(BINDIR)/ArduinoServer: CPPLogger/CPPLogger.h $(BUILDDIR)/TCPManager.o $(BUILDDIR)/DeviceManager.o $(BUILDDIR)/CountDownLatch.o 
	$(CC) $(CFLAGS) $(INCLUDE) build/* SerialPacketConn/build/* src/ArduinoServer.cpp -o $@
	chmod 755 $@

$(BUILDDIR)/CountDownLatch.o: include/CountDownLatch.h src/CountDownLatch.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c src/CountDownLatch.cpp -o $@

$(BUILDDIR)/Authenticator.o: include/Authenticator.h include/AuthenticationScheme.h src/Authenticator.cpp $(BUILDDIR)/SharedSecretScheme.o
	$(CC) $(CFLAGS) $(INCLUDE) -c src/Authenticator.cpp -o $@

$(BUILDDIR)/SharedSecretScheme.o: include/AuthenticationScheme.h include/SharedSecretScheme.h src/SharedSecretScheme.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c src/SharedSecretScheme.cpp -o $@

$(BUILDDIR)/TCPConn.o: include/TCPConn.h src/TCPConn.cpp CPPLogger/CPPLogger.h include/TCPPacketConn.h include/Authenticator.h $(BUILDDIR)/Authenticator.o $(BUILDDIR)/TCPPacketConn.o
	$(CC) $(CFLAGS) $(INCLUDE) -c src/TCPConn.cpp -o $@

$(BUILDDIR)/TCPPacketConn.o: include/TCPPacketConn.h src/TCPPacketConn.cpp CPPLogger/CPPLogger.h
	$(CC) $(CFLAGS) $(INCLUDE) -c src/TCPPacketConn.cpp -o $@

$(BUILDDIR)/TCPManager.o: include/TCPManager.h src/TCPManager.cpp include/RequestObj.h CPPLogger/CPPLogger.h include/TCPConn.h $(BUILDDIR)/TCPConn.o
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
	rm -f $(BINDIR)/*
	$(MAKE) -C SerialPacketConn clean
