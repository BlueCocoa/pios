//
//  dummy.cpp
//  pios-mac
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <sys/types.h>

int main() {
    return 0;
}

/**
 Read 1 byte from given memory address

 @param address memory address
 @return exactly 1 byte from given memory address
 */
extern "C" uint32_t GET32(unsigned long address) {
    return 1;
}


/**
 Store 1 byte to given memory address

 @param address memory address
 @param value the value to store in the given memory address
 */
extern "C" void PUT32(unsigned long address, unsigned int value) {
}

extern "C"
void wait_op(void) {
    
}

extern "C"
void delay(uint32_t) {

}
