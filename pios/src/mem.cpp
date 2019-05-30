//
//  mem.c
//  pios-mac
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <pi/mem.h>
#include <sys/types.h>
#include <asm/barrier.h>

using namespace pi;

/**
 memory barrier
 */
volatile void mem::barrier() {
#if defined(__APPLE__)
#else
    rmb();
    wmb();
    dma_rmb();
    dma_wmb();
#endif
}

/**
 move given memory to destination address

 @param dst destination address
 @param src source address
 @param length length in bytes
 @return destination address
 */
void * mem::memmove(void * dst, const void * src, uint32_t length) {
    // return immediately if length equals 0
    if (length == 0) {
        return dst;
    }

    // turn destination and source pointers into integers for easier calculations
    register uint64_t d = (uint64_t)dst;
    register uint64_t s = (uint64_t)src;

    // Assume the memory blocks are word aligned. Most will be, and the
    // CPU can deal with unaligned accesses if necessary (as long as it
    // is configured to do so)
    if (d > s && d < (s + length)) {
        // Destination starts inside source area - work backwards

        // If length isn't a multiple of 4 bytes, copy the last 1-3 bytes first
        while (length & 3) {
            length--;
            ((unsigned char *)d)[length] = ((unsigned char *)s)[length];
        }

        // Copy everything else as 32-bit words. If one or both of the
        // memory areas aren't aligned, this will cause unaligned
        // reads. Inefficient, but less so than doing everything as
        // a series of bytes
        while (length) {
            length -= 4;
            *((uint32_t *)d + length) = *((uint32_t *)s + length);
        }
    } else {
        // Source starts inside destination area - working forwards
        // is fine - or two areas don't overlap (or they overlap
        // exactly, but that's an unlikely edge case)
        //
        // Copy as much as possible as 32-bit words. See above for
        // alignment issues
        while (length & 0xFFFFFFFC) {
            *((uint32_t *)d) = *((uint32_t *)s);
            d += 4;
            s += 4;
            length -= 4;
        }

        // Deal with 1-3 remaining bytes, if applicable
        while (length) {
            *((unsigned char *)d) = *((unsigned char *)s);
            d++;
            s++;
            length--;
        }
    }

    return dst;
}

/**
 write zeroes to a byte string

 @param addr starting address
 @param length n zeroed bytes
 */
void mem::bzero(void * addr, uint64_t length) {
    register uint64_t address = (uint64_t)addr;

    // If the start address is unaligned, fill in the first 1-3 bytes until it is
    while ((address & 3) && length) {
        *((unsigned char *)address) = 0;
        address++;
        length--;
    }

    // Fill in the remaining 32-bit word-aligned memory locations
    while (length & 0xFFFFFFFC) {
        *((uint32_t *)address) = 0;
        address += 4;
        length -= 4;
    }

    // Deal with the remaining 1-3 bytes, if any
    while (length) {
        address++;
        length--;
        *((unsigned char *)address) = 0;
    }
}
