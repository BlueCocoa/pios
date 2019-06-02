//
//  uart.h
//  pios-mac
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#ifndef __PI_URAT_H
#define __PI_URAT_H

#include <pi/peripherals.h>
#include <sys/types.h>

#define AUX_BASE            (ARM_PERIPHERALS_BASE + 0x215000)
#define AUX_IRQ             (AUX_BASE + 0x00)
#define AUX_ENABLES         (AUX_BASE + 0x04)
#define AUX_MU_IO_REG       (AUX_BASE + 0x40)
#define AUX_MU_IER_REG      (AUX_BASE + 0x44)
#define AUX_MU_IIR_REG      (AUX_BASE + 0x48)
#define AUX_MU_LCR_REG      (AUX_BASE + 0x4C)
#define AUX_MU_MCR_REG      (AUX_BASE + 0x50)
#define AUX_MU_LSR_REG      (AUX_BASE + 0x54)
#define AUX_MU_MSR_REG      (AUX_BASE + 0x58)
#define AUX_MU_SCRATCH      (AUX_BASE + 0x5C)
#define AUX_MU_CNTL_REG     (AUX_BASE + 0x60)
#define AUX_MU_STAT_REG     (AUX_BASE + 0x64)
#define AUX_MU_BAUD_REG     (AUX_BASE + 0x68)
#define AUX_SPI0_CNTL0_REG  (AUX_BASE + 0x80)
#define AUX_SPI0_CNTL1_REG  (AUX_BASE + 0x84)
#define AUX_SPI0_STAT_REG   (AUX_BASE + 0x88)
#define AUX_SPI0_IO_REG     (AUX_BASE + 0x90)
#define AUX_SPI0_PEEK_REG   (AUX_BASE + 0x94)
#define AUX_SPI1_CNTL0_REG  (AUX_BASE + 0xC0)
#define AUX_SPI1_CNTL1_REG  (AUX_BASE + 0xC4)
#define AUX_SPI1_STAT_REG   (AUX_BASE + 0xC8)
#define AUX_SPI1_IO_REG     (AUX_BASE + 0xD0)
#define AUX_SPI1_PEEK_REG   (AUX_BASE + 0xD4)

namespace pi {

namespace UART {
    enum {
        BAUD_115200 = 270,
        BAUD_57600  = 541,
        BAUD_19200  = 1626,
        BAUD_9600   = 3254,
        BAUD_4800   = 6509,
    };

    /**
     initialize UART with given baudrate

     @param baudrate the baudrate for UART
     */
    void init(uint32_t baudrate = BAUD_115200);

    /**
     read one byte from UART

     @return exactly 1 byte in uint32_t
     */
    uint32_t read_byte();

    /**
     read until encounters given terminator

     @param str a pointer to a string which has pre-allocated free space
     @param terminator terminator
     */
    void read_until(char * str, char terminator);

    /**
     read whole line

     @param str a pointer to a string which has pre-allocated free space
     */
    void read_line(char * str);

    /**
     write one byte to UART

     @param byte the byte to write
     */
    void write_byte(const uint32_t byte);

    /**
     write string to UART

     @param str the string to write
     */
    void write(const char * str);

    /**
     write 32bit digit in hex to UART

     @param data the digit to write
     */
    void write_digit32(uint32_t data);

    /**
     write 64bit digit in hex to UART

     @param data the digit to write
     */
    void write_digit64(uint64_t data);

    /**
     write string to UART with extra '\r\n' at last

     @param str the string to write
     */
    void write_line(const char * str);
}

}

#endif /* __PI_URAT_H */
