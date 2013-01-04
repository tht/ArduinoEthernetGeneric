/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef	ETHERNETW5200_H_INCLUDED
#define	ETHERNETW5200_H_INCLUDED

#include "EthernetClass.h"

#define MAX_SOCK_NUM 8

typedef uint8_t SOCKET;

class SnMR {
public:
    static const uint8_t CLOSE  = 0x00;
    static const uint8_t TCP    = 0x01;
    static const uint8_t UDP    = 0x02;
    static const uint8_t IPRAW  = 0x03;
    static const uint8_t MACRAW = 0x04;
    static const uint8_t PPPOE  = 0x05;
    static const uint8_t ND     = 0x20;
    static const uint8_t MULTI  = 0x80;
};

enum SockCMD {
    Sock_OPEN      = 0x01,
    Sock_LISTEN    = 0x02,
    Sock_CONNECT   = 0x04,
    Sock_DISCON    = 0x08,
    Sock_CLOSE     = 0x10,
    Sock_SEND      = 0x20,
    Sock_SEND_MAC  = 0x21,
    Sock_SEND_KEEP = 0x22,
    Sock_RECV      = 0x40
};

class SnIR {
public:
    static const uint8_t SEND_OK = 0x10;
    static const uint8_t TIMEOUT = 0x08;
    static const uint8_t RECV    = 0x04;
    static const uint8_t DISCON  = 0x02;
    static const uint8_t CON     = 0x01;
};

class SnSR {
public:
    static const uint8_t CLOSED      = 0x00;
    static const uint8_t INIT        = 0x13;
    static const uint8_t LISTEN      = 0x14;
    static const uint8_t SYNSENT     = 0x15;
    static const uint8_t SYNRECV     = 0x16;
    static const uint8_t ESTABLISHED = 0x17;
    static const uint8_t FIN_WAIT    = 0x18;
    static const uint8_t CLOSING     = 0x1A;
    static const uint8_t TIME_WAIT   = 0x1B;
    static const uint8_t CLOSE_WAIT  = 0x1C;
    static const uint8_t LAST_ACK    = 0x1D;
    static const uint8_t UDP         = 0x22;
    static const uint8_t IPRAW       = 0x32;
    static const uint8_t MACRAW      = 0x42;
    static const uint8_t PPPOE       = 0x5F;
};

class IPPROTO {
public:
    static const uint8_t IP   = 0;
    static const uint8_t ICMP = 1;
    static const uint8_t IGMP = 2;
    static const uint8_t GGP  = 3;
    static const uint8_t TCP  = 6;
    static const uint8_t PUP  = 12;
    static const uint8_t UDP  = 17;
    static const uint8_t IDP  = 22;
    static const uint8_t ND   = 77;
    static const uint8_t RAW  = 255;
};


class EthernetW5200 : public EthernetClass {
    
public:
    void init();
    
    void read_data(SOCKET s, uint16_t src, volatile uint8_t * dst, uint16_t len);
    void send_data_processing(SOCKET s, const uint8_t *data, uint16_t len);
    void send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len);

    void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek = 0);
    
    void setMACAddress(uint8_t * addr);
    void getMACAddress(uint8_t * addr);
    void setIPAddress(uint8_t * addr);
    void getIPAddress(uint8_t * addr);
    void setSubnetMask(uint8_t *_addr);
    void getSubnetMask(uint8_t *_addr);
    void setGatewayIp(uint8_t *_addr);
    void getGatewayIp(uint8_t *_addr);

    inline void setRetransmissionTime(uint16_t timeout);
    inline void setRetransmissionCount(uint8_t _retry);
    
    void execCmdSn(SOCKET s, SockCMD _cmd);
    
    uint16_t getTXFreeSize(SOCKET s);
    uint16_t getRXReceivedSize(SOCKET s);
    
    
    // W5200 Registers
    // ---------------
private:
    static uint8_t write(uint16_t _addr, uint8_t _data);
    static uint16_t write(uint16_t addr, const uint8_t *buf, uint16_t len);
    static uint8_t read(uint16_t addr);
    static uint16_t read(uint16_t addr, uint8_t *buf, uint16_t len);
    
#define __GP_REGISTER8(name, address)                  \
  static inline void write##name(uint8_t _data) {      \
    write(address, _data);                             \
  }                                                    \
  static inline uint8_t read##name() {                 \
    return read(address);                              \
  }
#define __GP_REGISTER16(name, address)                 \
  static inline void write##name(uint16_t _data) {     \
    write(address, (uint8_t*) &_data, 2);              \
  }                                                    \
  static inline uint16_t read##name() {                \
    uint16_t res;                                      \
    read(address, (uint8_t*) &res, 2);                 \
    return res;                                        \
}
#define __GP_REGISTER_N(name, address, size)           \
  static inline uint16_t write##name(uint8_t *_buff) { \
    return write(address, _buff, size);                \
  }                                                    \
  static uint16_t read##name(uint8_t *_buff) {         \
    return read(address, _buff, size);                 \
}
    
public:
    __GP_REGISTER8 (MR,     0x0000);    // Mode
    __GP_REGISTER_N(GAR,    0x0001, 4); // Gateway IP address
    __GP_REGISTER_N(SUBR,   0x0005, 4); // Subnet mask address
    __GP_REGISTER_N(SHAR,   0x0009, 6); // Source MAC address
    __GP_REGISTER_N(SIPR,   0x000F, 4); // Source IP address
    __GP_REGISTER8 (IR,     0x0015);    // Interrupt
    __GP_REGISTER8 (IMR,    0x0016);    // Interrupt Mask
    __GP_REGISTER16(RTR,    0x0017);    // Timeout
    __GP_REGISTER8 (RCR,    0x0019);    // Retry count
    __GP_REGISTER8 (PATR,   0x001C);    // Authentication type in PPPoE mode
    __GP_REGISTER8 (PPPALGO,0x001E);    // Authentication algorithm in PPPoE mode
    __GP_REGISTER8 (VERSIONNR,0x001F);  // W5200 Chip Version Register
    __GP_REGISTER_N(UIPR,   0x002A, 4); // Unreachable IP address in UDP mode
    __GP_REGISTER16(UPORT,  0x002E);    // Unreachable Port address in UDP mode
    __GP_REGISTER8 (PTIMER, 0x0028);    // PPP LCP Request Timer
    __GP_REGISTER8 (PMAGIC, 0x0029);    // PPP LCP Magic Number
    __GP_REGISTER16(INTLEVEL,0x0030);   // Interrupt Low Level Timer Register
    __GP_REGISTER8 (IR2,    0x0034);    // W5200 SOCKET Interrupt Register
    __GP_REGISTER8 (PHYSTATUS,0x0035);  // W5200 PHY status Register
    __GP_REGISTER8 (IMR2,   0x0036);    // Interrupt Mask Register2
    
#undef __GP_REGISTER8
#undef __GP_REGISTER16
#undef __GP_REGISTER_N
    
    
    // W5200 Socket registers
    // ----------------------
private:
    static inline uint8_t readSn(SOCKET _s, uint16_t _addr);
    static inline uint8_t writeSn(SOCKET _s, uint16_t _addr, uint8_t _data);
    static inline uint16_t readSn(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t len);
    static inline uint16_t writeSn(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t len);
    
    static const uint16_t CH_BASE = 0x4000;
    static const uint16_t CH_SIZE = 0x0100;
    
#define __SOCKET_REGISTER8(name, address)                         \
  static inline void write##name(SOCKET _s, uint8_t _data) {      \
    writeSn(_s, address, _data);                                  \
  }                                                               \
  static inline uint8_t read##name(SOCKET _s) {                   \
    return readSn(_s, address);                                   \
}
#define __SOCKET_REGISTER16(name, address)                        \
  static inline void write##name(SOCKET _s, uint16_t _data) {     \
    writeSn(_s, address,   _data >> 8);                           \
    writeSn(_s, address+1, _data & 0xFF);                         \
  }                                                               \
  static inline uint16_t read##name(SOCKET _s) {                  \
    uint16_t res = readSn(_s, address);                           \
    uint16_t res2 = readSn(_s,address + 1);                       \
    res = res << 8;                                               \
    res2 = res2 & 0xFF;                                           \
    res = res | res2;                                             \
    return res;                                                   \
}
#define __SOCKET_REGISTER_N(name, address, size)                  \
  static inline uint16_t write##name(SOCKET _s, uint8_t *_buff) { \
    return writeSn(_s, address, _buff, size);                     \
  }                                                               \
  static inline uint16_t read##name(SOCKET _s, uint8_t *_buff) {  \
    return readSn(_s, address, _buff, size);                      \
}
    
public:
    __SOCKET_REGISTER8(SnMR,        0x0000)        // Mode
    __SOCKET_REGISTER8(SnCR,        0x0001)        // Command
    __SOCKET_REGISTER8(SnIR,        0x0002)        // Interrupt
    __SOCKET_REGISTER8(SnSR,        0x0003)        // Status
    __SOCKET_REGISTER16(SnPORT,     0x0004)        // Source Port
    __SOCKET_REGISTER_N(SnDHAR,     0x0006, 6)     // Destination Hardw Addr
    __SOCKET_REGISTER_N(SnDIPR,     0x000C, 4)     // Destination IP Addr
    __SOCKET_REGISTER16(SnDPORT,    0x0010)        // Destination Port
    __SOCKET_REGISTER16(SnMSSR,     0x0012)        // Max Segment Size
    __SOCKET_REGISTER8(SnPROTO,     0x0014)        // Protocol in IP RAW Mode
    __SOCKET_REGISTER8(SnTOS,       0x0015)        // IP TOS
    __SOCKET_REGISTER8(SnTTL,       0x0016)        // IP TTL
    __SOCKET_REGISTER8(SnRXMEM_SIZE,0x001E)        // Internal RX Memory size in kb
    __SOCKET_REGISTER8(SnTXMEM_SIZE,0x001F)        // Internal TX Memory size in kb
    __SOCKET_REGISTER16(SnTX_FSR,   0x0020)        // TX Free Size
    __SOCKET_REGISTER16(SnTX_RD,    0x0022)        // TX Read Pointer
    __SOCKET_REGISTER16(SnTX_WR,    0x0024)        // TX Write Pointer
    __SOCKET_REGISTER16(SnRX_RSR,   0x0026)        // RX Free Size
    __SOCKET_REGISTER16(SnRX_RD,    0x0028)        // RX Read Pointer
    __SOCKET_REGISTER16(SnRX_WR,    0x002A)        // RX Write Pointer (supported?)
    __SOCKET_REGISTER8(SnIMR,       0x002C)        // Interrupt Mask Register
    __SOCKET_REGISTER8(SnFRAG,      0x002D)        // Fragment Register
    
#undef __SOCKET_REGISTER8
#undef __SOCKET_REGISTER16
#undef __SOCKET_REGISTER_N
    

private:
    static const uint16_t SMASK = 0x07FF; // Tx buffer MASK
    static const uint16_t RMASK = 0x07FF; // Rx buffer MASK
    
public:
    static const uint16_t SSIZE = 2048; // Max Tx buffer size

private:
    static const uint16_t RSIZE = 2048; // Max Rx buffer size
    uint16_t SBASE[MAX_SOCK_NUM]; // Tx buffer base address
    uint16_t RBASE[MAX_SOCK_NUM]; // Rx buffer base address
};


#endif
