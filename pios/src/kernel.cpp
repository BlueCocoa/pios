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

    // set GPIO pin 16 for OUTPUT
    pi::gpio::set_function(16, pi::gpio::OUTPUT);

    // forever
    while (1) {
        // set high voltage
        pi::gpio::set(16, true);

        // sleep
        delay(100000);

        // set low voltage
        pi::gpio::set(16, false);

        // sleep
        delay(200000);
    }
}
