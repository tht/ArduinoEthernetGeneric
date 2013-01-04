/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "EthernetW5200.h"


// ==========================================================================
// Some configuration options

// SPI Transfer Speed (reduce for debugging, see datasheet p968ff)
#define SPI_CTAR 0xB8010000 // 16MHz
//#define SPI_CTAR 0xB8000000 // 24MHz

// SlaveSelect pin for W55200
#define W5200SS 9

// End of configuration options
// ==========================================================================


#define TX_RX_MAX_BUF_SIZE 2048
#define TX_BUF 0x1100
#define RX_BUF (TX_BUF + TX_RX_MAX_BUF_SIZE)

#define TXBUF_BASE 0x8000
#define RXBUF_BASE 0xC000


uint8_t EthernetW5200::readSn(SOCKET _s, uint16_t _addr) {
    return read(CH_BASE + _s * CH_SIZE + _addr);
}

uint8_t EthernetW5200::writeSn(SOCKET _s, uint16_t _addr, uint8_t _data) {
    return write(CH_BASE + _s * CH_SIZE + _addr, _data);
}

uint16_t EthernetW5200::readSn(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
    return read(CH_BASE + _s * CH_SIZE + _addr, _buf, _len);
}

uint16_t EthernetW5200::writeSn(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
    return write(CH_BASE + _s * CH_SIZE + _addr, _buf, _len);
}

void EthernetW5200::getGatewayIp(uint8_t *_addr)  { readGAR(_addr);   }
void EthernetW5200::setGatewayIp(uint8_t *_addr)  { writeGAR(_addr);  }
void EthernetW5200::getSubnetMask(uint8_t *_addr) { readSUBR(_addr);  }
void EthernetW5200::setSubnetMask(uint8_t *_addr) { writeSUBR(_addr); }
void EthernetW5200::getMACAddress(uint8_t *_addr) { readSHAR(_addr);  }
void EthernetW5200::setMACAddress(uint8_t *_addr) { writeSHAR(_addr); }
void EthernetW5200::getIPAddress(uint8_t *_addr)  { readSIPR(_addr);  }
void EthernetW5200::setIPAddress(uint8_t *_addr)  { writeSIPR(_addr); }
void EthernetW5200::setRetransmissionTime(uint16_t _timeout) { writeRTR(_timeout); }
void EthernetW5200::setRetransmissionCount(uint8_t _retry)   { writeRCR(_retry); }



void EthernetW5200::init(void) {
    
    // configure SPI pins
    digitalWriteFast(SCK, LOW);
    digitalWriteFast(MOSI, LOW);
    digitalWriteFast(W5200SS, HIGH);
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, OUTPUT);
    pinMode(W5200SS, OUTPUT);
    
    // enables and configures SPI module
    SIM_SCGC6 |= SIM_SCGC6_SPI0;  // enable SPI clock
    SPI0_MCR = 0x80004000;
    SPCR |= _BV(MSTR);
    SPCR |= _BV(SPE);
    SPCR &= ~(_BV(DORD));   // MSBFIRST SPI
    
    // resetting W5200
    writeMR(1<<7);
    
    for (int i=0; i<MAX_SOCK_NUM; i++) {
        SBASE[i] = TXBUF_BASE + SSIZE * i;
        RBASE[i] = RXBUF_BASE + RSIZE * i;
    }
}


uint16_t EthernetW5200::getTXFreeSize(SOCKET s) {
    uint16_t val=0, val1=0;
    do {
        val1 = readSnTX_FSR(s);
        if (val1 != 0)
            val = readSnTX_FSR(s);
    } while (val != val1);
    return val;
}


uint16_t EthernetW5200::getRXReceivedSize(SOCKET s) {
    uint16_t val=0,val1=0;
    do {
        val1 = readSnRX_RSR(s);
        if (val1 != 0)
            val = readSnRX_RSR(s);
    } while (val != val1);
    return val;
}


void EthernetW5200::send_data_processing(SOCKET s, const uint8_t *data, uint16_t len) {
    // This is same as having no offset in a call to send_data_processing_offset
    send_data_processing_offset(s, 0, data, len);
}


void EthernetW5200::send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len) {
    uint16_t ptr = readSnTX_WR(s);
    ptr += data_offset;
    uint16_t offset = ptr & SMASK;
    uint16_t dstAddr = offset + SBASE[s];
    
    if (offset + len > SSIZE) {
        // Wrap around circular buffer
        uint16_t size = SSIZE - offset;
        write(dstAddr, data, size);
        write(SBASE[s], data + size, len - size);
    } else {
        write(dstAddr, data, len);
    }
    
    ptr += len;
    writeSnTX_WR(s, ptr);
}


void EthernetW5200::recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek) {
    uint16_t ptr;
    ptr = readSnRX_RD(s);
    read_data(s, ptr, data, len);
    if (!peek) {
        ptr += len;
        writeSnRX_RD(s, ptr);
    }
}


void EthernetW5200::read_data(SOCKET s, uint16_t src, volatile uint8_t *dst, uint16_t len) {
    uint16_t size;
    uint16_t src_mask;
    uint16_t src_ptr;

    src_mask = (uint16_t)src & RMASK;
    src_ptr = RBASE[s] + src_mask;

    if( (src_mask + len) > RSIZE ) {
        size = RSIZE - src_mask;
        read(src_ptr, (uint8_t *)dst, size);
        dst += size;
        read(RBASE[s], (uint8_t *) dst, len - size);
    } else {
        read(src_ptr, (uint8_t *) dst, len);
    }
}


uint8_t EthernetW5200::write(uint16_t addr, uint8_t data) {
    return write(addr, &data, 1);
}


uint16_t EthernetW5200::write(uint16_t addr, const uint8_t *data, uint16_t data_len) {
    noInterrupts();

    // 8bit transfers
    SPI0_CTAR0 = SPI_CTAR;
    digitalWriteFast(W5200SS, LOW);
    
    SPI0_PUSHR = (1<<26) | ((addr & 0xFF00) >> 8);
    SPI0_PUSHR =           ( addr & 0xFF);
    
    SPI0_PUSHR =           0x80 | ((data_len & 0x7F00) >> 8);
    SPI0_PUSHR =           ( data_len & 0xFF);
    
    for (int i=0; i<data_len; i++) {
        while ((0xF & (SPI0_SR >> 12)) == 4) ; // wait while TX fifo is full
        SPI0_PUSHR =        *(data+i);
    }
    
    // add 4 control bytes to data_len and shift to match SPI0_TCR
    uint32_t data_sent = data_len + 4;
    data_sent <<= 16;
    while (SPI0_TCR != data_sent) ; // loop until transfer is complete
    
    digitalWriteFast(9, HIGH);
    interrupts();

    return data_len;
}


uint8_t EthernetW5200::read(uint16_t _addr) {
    uint8_t res;
    read(_addr, &res, 1);
    return res;
}


uint16_t EthernetW5200::read(uint16_t addr, uint8_t *data, uint16_t data_len) {
    noInterrupts();

    // 8bit transfers
    SPI0_CTAR0 = SPI_CTAR;
    digitalWriteFast(W5200SS, LOW);
    
    SPI0_PUSHR = (1<<26) | ((addr & 0xFF00) >> 8);
    SPI0_PUSHR =           ( addr & 0xFF);
    
    SPI0_PUSHR =           0x00 | ((data_len & 0x7F00) >> 8);
    SPI0_PUSHR =           ( data_len & 0xFF);
    
    while ((0xF & (SPI0_SR >> 12)) != 0) ; // wait until all is sent

    while ((0xF & (SPI0_SR >> 4)) != 0)    // discard everything from RX FIFO
        uint8_t discard = SPI0_POPR;
    
    for (int i=0; i<data_len; i++) {
        SPI0_PUSHR = 0;
        while ( ! (0xF & (SPI0_SR >> 4)) ) ; // wait until we get a byte
        data[i] = SPI0_POPR;
    }
    
    digitalWriteFast(9, HIGH);
    interrupts();

    return data_len;
}


void EthernetW5200::execCmdSn(SOCKET s, SockCMD _cmd) {
    // Send command to socket
    writeSnCR(s, _cmd);

    // Wait for command to complete
    while (readSnCR(s))
        ;
}
