//
//  mem.h
//  pios-mac
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#ifndef __PI_MEM_H
#define __PI_MEM_H

#include <sys/types.h>

/**
 Read 1 byte from given memory address

 @param address memory address
 @return exactly 1 byte from given memory address
 */
extern "C" uint32_t GET32(unsigned long address);


/**
 Store 1 byte to given memory address

 @param address memory address
 @param value the value to store in the given memory address
 */
extern "C" void PUT32(unsigned long address, unsigned int value);

#endif /* __PI_MEM_H */
