//
//  uart.cpp
//  pios-mac
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <pi/uart.h>
#include <pi/gpio.h>
#include <pi/mem.h>
#include <pi/extern.h>

using namespace pi;

/**
 initialize UART with given baudrate

 @param baudrate the baudrate for UART
 */
void UART::init(uint32_t baudrate) {
    unsigned int ra;
    PUT32(AUX_ENABLES, 1);
    PUT32(AUX_MU_IER_REG, 0);
    PUT32(AUX_MU_CNTL_REG, 0);
    PUT32(AUX_MU_LCR_REG, 3);
    PUT32(AUX_MU_MCR_REG, 0);
    PUT32(AUX_MU_IER_REG, 0);
    PUT32(AUX_MU_IIR_REG, 0xC6);
    PUT32(AUX_MU_BAUD_REG, baudrate);
    ra = GET32(GPFSEL1);
    ra &= ~(7 << 12);
    ra |= 2 << 12;
    ra &= ~(7 << 15);
    ra |= 2 << 15;
    PUT32(GPFSEL1, ra);
    PUT32(GPPUD, 0);
    for (ra = 0; ra < 150; ra++) {
        wait_op();
    }
    PUT32(GPPUDCLK0, (1 << 14) | (1 << 15));
    for (ra = 0; ra < 150; ra++) {
        wait_op();
    }
    PUT32(GPPUDCLK0, 0);
    PUT32(AUX_MU_CNTL_REG, 3);
}

/**
 read one byte from UART

 @return exactly 1 byte in uint32_t
 */
uint32_t UART::read_byte() {
    while (1) {
        if (GET32(AUX_MU_LSR_REG) & 0x01) {
            break;
        }
    }

    return(GET32(AUX_MU_IO_REG) & 0xFF);
}

/**
 read until encounters given terminator or n bytes at max
 
 @param str a pointer to a string which has pre-allocated free space
 @param terminator terminator
 */
uint64_t UART::readn_until(char * str, char terminator, uint32_t n) {
    if (n == 0) {
        return 0;
    }
    
    uint64_t length = 0;
    uint8_t byte = read_byte();
    while (byte != terminator) {
        str[length] = byte;
        length++;
        if (length == n) {
            break;
        }
        byte = read_byte();
    }
    str[length] = '\0';
    return length;
}

/**
 read until encounters given terminator

 @param str a pointer to a string which has pre-allocated free space
 @param terminator terminator
 */
void UART::read_until(char * str, char terminator) {
    uint64_t length = 0;
    uint8_t byte = read_byte();
    while (byte != terminator) {
        str[length] = byte;
        length++;
        byte = read_byte();
    }
    str[length] = '\0';
}


/**
 read whole line

 @param str a pointer to a string which has pre-allocated free space
 */
void UART::read_line(char * str) {
    read_until(str, '\n');
}

/**
 write one byte to UART

 @param byte the byte to write
 */
void UART::write_byte(const uint32_t byte) {
    while (1) {
        if (GET32(AUX_MU_LSR_REG) & 0x20) {
            break;
        }
    }
    PUT32(AUX_MU_IO_REG, byte);
}


/**
 write string to UART

 @param str the string to write
 */
void UART::write(const char * str) {
    uint32_t pos = 0;
    for (; str[pos] != '\0'; pos++) {
        write_byte(str[pos]);
    }
}

/**
 write 32bit digit in hex to UART

 @param data the digit to write
 */
void UART::write_digit32(uint32_t data) {
    unsigned int rb;
    unsigned int rc;
    rb = 32;
    while (1) {
        rb -= 4;
        rc = (data >> rb) & 0xF;
        if(rc > 9) rc += 0x37;
        else rc += 0x30;
        write_byte(rc);
        if(rb == 0) break;
    }
}

/**
 write 64bit digit in hex to UART

 @param data the digit to write
 */
void UART::write_digit64(uint64_t data) {
    unsigned int rb;
    unsigned int rc;
    rb = 64;
    while (1) {
        rb -= 4;
        rc = (data >> rb) & 0xF;
        if(rc > 9) rc += 0x37;
        else rc += 0x30;
        write_byte(rc);
        if(rb == 0) break;
    }
}

/**
 write string to UART with extra '\r\n' at last

 @param str the string to write
 */
void UART::write_line(const char * str) {
    write(str);
    write_byte('\r');
    write_byte('\n');
}
