#include "EthernetClass.h"

uint8_t EthernetClass::begin() {
	uint8_t mac[6];  // place to store mac address
//	read_mac(mac);
	begin(mac);
}

uint8_t EthernetClass::begin(uint8_t *mac_address) {
    /*
  _dhcp = new DhcpClass();

  // Initialise the basic info
  W5200.init();
  W5200.setMACAddress(mac_address);
  W5200.setIPAddress(IPAddress(0,0,0,0).raw_address());

  // Now try to get our config info from a DHCP server
  int ret = _dhcp->beginWithDHCP(mac_address);
  if(ret == 1)
  {
    // We've successfully found a DHCP server and got our configuration info, so set things
    // accordingly
    W5200.setIPAddress(_dhcp->getLocalIp().raw_address());
    W5200.setGatewayIp(_dhcp->getGatewayIp().raw_address());
    W5200.setSubnetMask(_dhcp->getSubnetMask().raw_address());
    _dnsServerAddress = _dhcp->getDnsServerIp();
  }

  return ret;
     */
    return 0;
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip) {
    // Assume the DNS server will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress dns_server = local_ip;
    dns_server[3] = 1;
    begin(mac_address, local_ip, dns_server);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server) {
    // Assume the gateway will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress gateway = local_ip;
    gateway[3] = 1;
    begin(mac_address, local_ip, dns_server, gateway);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway) {
    IPAddress subnet(255, 255, 255, 0);
    begin(mac_address, local_ip, dns_server, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet) {
  init();
  setMACAddress(mac);
  setIPAddress(local_ip._address);
  setGatewayIp(gateway._address);
  setSubnetMask(subnet._address);
  _dnsServerAddress = dns_server;
}

uint8_t EthernetClass::maintain(){
    /*
  int rc = DHCP_CHECK_NONE;
  if(_dhcp != NULL){
    //we have a pointer to dhcp, use it
    rc = _dhcp->checkLease();
    switch ( rc ){
      case DHCP_CHECK_NONE:
        //nothing done
        break;
      case DHCP_CHECK_RENEW_OK:
      case DHCP_CHECK_REBIND_OK:
        //we might have got a new IP.
        W5200.setIPAddress(_dhcp->getLocalIp().raw_address());
        W5200.setGatewayIp(_dhcp->getGatewayIp().raw_address());
        W5200.setSubnetMask(_dhcp->getSubnetMask().raw_address());
        _dnsServerAddress = _dhcp->getDnsServerIp();
        break;
      default:
        //this is actually a error, it will retry though
        break;
    }
  }
  return rc;
     */
    return 0;
}

IPAddress EthernetClass::localIP() {
    IPAddress ret;
    getIPAddress(ret.raw_address());
    return ret;
}

IPAddress EthernetClass::subnetMask() {
    IPAddress ret;
    getSubnetMask(ret.raw_address());
    return ret;
}

IPAddress EthernetClass::gatewayIP() {
    IPAddress ret;
    getGatewayIp(ret.raw_address());
    return ret;
}

IPAddress EthernetClass::dnsServerIP() {
    return _dnsServerAddress;
}
