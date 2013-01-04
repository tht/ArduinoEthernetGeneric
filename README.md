# Arduino Ethernet Library

This is a fork of the current (Arduino 1.0.2) Ethernet Library which should support multiple Ethernet Chipsets.

At the beginning this Library will support the WIZnet W5200 module as used on Wiz820io. Support for WIZnet W5100 should be trivial as the interface is almost identical. Later on hopefully other common chipsets will be supported too.

Usage should be almost as easy as it was with the existing library but some files and classes where renamed.

	// Always import a specific hardware-dependent implementation
	#include "EthernetW5200.h"
	
	// Include whatever protocols you need
	#include "TCPClient.h"

	// Assign hardware-dependent implementation to global variable
	EthernetW5200 Ethernet;
	
	void setup() {
	  // Use it as always
	  Ethernet.begin();
	}
	
	void loop() {
	  // do something useful here
	}


## Architecture
The reach this goal a encapsulation in different layers is needed. Most of the source code is already here but it's coupled too tight.

The following classes will be implemented.

### Hardware Independent Layer
	Classname: EthernetClass
	Parent-Class: (none)
	
	Methods available for next layer:
	- void      set{MAC|IP|Netmask|Gateway}(IPAddress);
	- IPAddress get{MAC|IP|Netmask|Gateway};
	- TCPSocket getTCPSocket();
	- UDPSocket getUDPSocket();

### Hardware Dependent Layer
	Classname:    EthernetW5200, EthernetW5100, …
	Parent-Class: EthernetClass
	Bound to global variable: Ethernet
	
	Methods available for next layer:
	- uint8_t   init();
	
	  // management commands
	- void      set{MAC|IP|Netmask|Gateway}(IPAddress);
	- IPAddress get{MAC|IP|Netmask|Gateway};
	
	  // Socket specific commands
	- uint8_t   getFreeSocketID();
	- void      releaseSocketID(uint8_t socketID);
	- uint8_t   setSocketType();
	- uint8_t   getSocketState();
	- uint16_t  getFreeTX(uint8_t socketID);
	- uint16_t  getAvailableRX(uint8_t socketID);
	
### Socket Layer
Common parent class:

	Classname: EthernetSocket
	Parent-Class: (none)
	
	Methods available for next layer:

TCP Socket:

	Classname: TCPSocket
	Parent-Class: EthernetSocket
	
	Methods available for next layer:
	  // Connecting to a server and transmitting data
	- uint8_t   connect(IPAddress server, uint16_t port);
	- void      disconnect();
	- uint16_t  send(uint8_t *buffer, uint16_t buffer_len);
	- uint16_t  recv(uint8_t *buffer, uint16_t buffer_len);

      // Start a server port
	- uint8_t   listen(uint16_t port);
	- TCPSocket accept();

UDP Socket:

	Classname: UDPSocket
	Parent-Class: EthernetSocket
	
	Methods available for next layer:
	  // Sending an UDP packet
	- uint8_t   startUDP(IPAddress addr, uint16_t port); // start preparing UDP packet
	- uint16_t  bufferData(uint8_t *buffer, uint16_t buffer_len); // add data to current packet
	- int       sendUDP();       // sends a buffered packet
	
	  // Receiving an UDP packet
	- uint16_t  receivePacket(); // Receive a packet, returns 0 there is none
	- uint16_t  available();     // Returns number of bytes waiting in current packet
	- uint8_t   read();          // reads one byte from current packet
	- uint16_t  read(uint8_t *buffer, uint16_t buffer_len); // reads data from current packet
	

### TCP/UDP Layers
Same interface as existing Arduino Ethernet Library. Documented in different source-files:

* TCP Client: EthernetClient.h
* TCP Server: EthernetServer.h
* UDP Socket: EthernetUdp.h

**Notice: The naming of these files and classes will probably change!**


## Existing high-level protocols
There are many implementations available in the Internet. Most of them should work. Included are:

* DHCP
* DNS