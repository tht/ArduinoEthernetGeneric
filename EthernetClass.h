#ifndef ethernet_h
#define ethernet_h

#include <Arduino.h>
#include "IPAddress.h"
//#include "Dhcp.h"

class MACAddress {
    
};

class EthernetClass {

private:
    IPAddress _dnsServerAddress;
    //  DhcpClass* _dhcp;

public:
    // Initialise the Ethernet shield to use the provided MAC address and gain the rest of the
    // configuration through DHCP.
    // Returns 0 if the DHCP configuration failed, and 1 if it succeeded
    uint8_t begin();
    uint8_t begin(uint8_t *mac_address);
    void begin(uint8_t *mac_address, IPAddress local_ip);
    void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server);
    void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
    void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
    uint8_t maintain();

    // convinience methods
    IPAddress  localIP();
    IPAddress  subnetMask();
    IPAddress  gatewayIP();
    IPAddress  dnsServerIP();
    MACAddress macAddress();
    
    // have to be defined in child class
    virtual void init() = 0;
    virtual void setMACAddress(uint8_t * addr) = 0;
    virtual void getMACAddress(uint8_t * addr) = 0;
    virtual void setIPAddress(uint8_t * addr) = 0;
    virtual void getIPAddress(uint8_t * addr) = 0;
    virtual void setSubnetMask(uint8_t *_addr) = 0;
    virtual void getSubnetMask(uint8_t *_addr) = 0;
    virtual void setGatewayIp(uint8_t *_addr) = 0;
    virtual void getGatewayIp(uint8_t *_addr) = 0;

    friend class EthernetClient;
    friend class EthernetServer;
};

#endif
