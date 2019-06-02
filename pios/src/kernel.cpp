//
//  kernel.cpp
//  pios
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <pi/pi.h>

extern "C"
void kernel_init(void) {
    // initilize UART
    pi::UART::init();

    // write something
    pi::UART::write_line("Hello");

    // initialize graphics
    bool succeeded = pi::graphics::init(1920, 1080, 32, false);

    if (succeeded) {
        pi::graphics::console::log("Cocoa!");
        pi::UART::write_line("succeeded");
    }

#ifndef __APPLE__
    asm volatile("wfi" : : : "memory");
#endif
}
