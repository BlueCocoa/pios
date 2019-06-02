//
//  mailbox.cpp
//  pios
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <pi/mailbox.h>
#include <pi/peripherals.h>
#include <pi/mem.h>

#ifdef DEBUG
#include <pi/uart.h>
#endif

using namespace pi;

static uint32_t RESPONSE_TABLE [] = { 0, 4, 6, 8, 16, 136, 1024 };
static uint32_t REQUEST_TABLE [] = { 0, 4, 8, 12, 16, 24, 28 };

/**
 read from mailbox

 @param channel mailbox channel

 @return value read from that channel
 */
uint32_t mailbox::read(uint32_t channel) {
#ifdef DEBUG
    UART::write("mailbox read channel: ");
    UART::write_digit32(channel);
    UART::write_line("");
#endif
    uint32_t ret;

    while (1) {
        // wait for mailbox if it reports empty
        while (empty()) { }

        // apply memory barrier before get
        mem::barrier();

        // read from mailbox
        ret = GET32(MAILBOX_BASE);

        // apply memory barrier after get
        mem::barrier();

        // break while statement
        // if the value came from the expected channel
        if ((ret & 0xF) == channel) {
            ret >>= 4;
            break;
        }
    }

#ifdef DEBUG
    UART::write("mailbox did read: ");
    UART::write_digit32(ret);
    UART::write_line("");
#endif

    return ret;
}

/**
 write to mailbox

 @param channel mailbox channel
 @param value value for writing
 */
void mailbox::write(uint32_t channel, uint32_t value) {
#ifdef DEBUG
    UART::write("mailbox write: ");
    UART::write_digit32((value << 4) | channel);
    UART::write_line("");
#endif

    // wait for mailbox if it reports full
    while (full()) { }

    // apply memory barrier before put
    mem::barrier();

    // write to mailbox
    PUT32(MAILBOX_WRITE, (value << 4) | channel);

#ifdef DEBUG
    UART::write("mailbox did write: ");
    UART::write_digit32((value << 4) | channel);
    UART::write_line("");
#endif
    // apply memory barrier after put
    mem::barrier();
}


/**
 is mailbox full

 @return true if full
 */
bool mailbox::full() {
    return GET32(MAILBOX_STATUS) & MAIL_FULL;
}

/**
 is mailbox empty

 @return true if empty
 */
bool mailbox::empty() {
    return GET32(MAILBOX_STATUS) & MAIL_EMPTY;
}

/**
 set mailbox property

 @param tag property tag
 @param args arguments
 
 @return mailbox response
 */
volatile uint32_t * mailbox::property(property_t tag, uint32_t * args) {
    // compute length mask
    uint8_t request_len_mask = (tag & (((1 << 6) - 1) << 20)) >> 20;
    uint8_t response_len_mask = (tag & (((1 << 6) - 1) << 26)) >> 26;

    // get tag id
    uint32_t tag_id = (tag << 12) >> 12;

    // mailbox message buffer
    static volatile uint32_t mailbox_buffer[64] = { 0 };

    // fill message buffer
    mailbox_buffer[0] = 24 + RESPONSE_TABLE[response_len_mask];
    mailbox_buffer[1] = 0;
    mailbox_buffer[2] = tag_id;
    mailbox_buffer[3] = RESPONSE_TABLE[response_len_mask];
    mailbox_buffer[4] = REQUEST_TABLE[request_len_mask];
    uint32_t arg;
    for (arg = 0; arg < (REQUEST_TABLE[request_len_mask] >> 2); arg++) {
        mailbox_buffer[5 + arg] = args[arg];
    }
    mailbox_buffer[5 + arg] = 0;

    uint32_t mailbox_buffer_ptr = (uint32_t)((uint64_t)&mailbox_buffer);
    write(8, mailbox_buffer_ptr);
    read(8);

#warning malloc memory for response
    if (mailbox_buffer[1] != 0x80000000) {
        mailbox_buffer[0] = 0;
        return mailbox_buffer;
    } else {
        mailbox_buffer[4] = (RESPONSE_TABLE[response_len_mask] >> 2);
        return mailbox_buffer + 4;
    }
}
