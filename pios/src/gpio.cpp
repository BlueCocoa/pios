//
//  gpio.cpp
//  pios
//
//  Created by Cocoa Oikawa on 2019/5/31.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <pi/gpio.h>
#include <pi/mem.h>
#include <sys/types.h>

using namespace pi;

/**
 set function of GPIO pin

 @param pin         GPIO pin
 @param function    one in the enum `pi::gpio::function_t`
 */
void gpio::set_function(uint32_t pin, gpio::function_t function) {
    // rpi don't have that much GPIO pins...
    if (pin > 53) {
        return;
    }

    // check argument
    if (function > 7) {
        return;
    }

    // compute corresponding GPIO address and bit index of requested pin
    uint32_t gpio_addr = GPIO_BASE;
    while (pin > 9) {
        pin -= 10;
        gpio_addr += 4;
    }
    pin = (pin << 1) + pin;

    // compute mask
    uint32_t func_mask = function << pin;
    uint32_t mask = 7 << pin;

    // get old mask
    uint32_t old_mask = GET32(gpio_addr);

    // append new mask onto old one
    mask = ~mask;
    old_mask &= mask;
    old_mask |= func_mask;

    // set mask
    PUT32(gpio_addr, old_mask);
}

/**
 turn on/off a GPIO pin

 @param pin     requested GPIO pin
 @param on      true for on, false for off
 */
void gpio::set(uint32_t pin, bool on) {
    // rpi don't have that much GPIO pins...
    if (pin > 53) {
        return;
    }

    // compute corresponding GPIO address of requested pin
    uint32_t gpio_addr = GPIO_BASE;
    uint32_t pin_bank = (pin >> 5) << 2;
    gpio_addr += pin_bank;

    // compute GPIO address for requested status setting
    if (on) {
        gpio_addr += 0x1C;
    } else {
        gpio_addr += 0x28;
    }

    // set mask
    uint32_t old_mask = GET32(gpio_addr);
    old_mask |= 1 << (pin & 31);
    PUT32(gpio_addr, old_mask);
}

/**
 set pull for GPIO pin

 @param pin     requested GPIO pin
 @param pull    one of the enum `pi::gpio::pull_t`
 @param on      true for on, false for off
 */
void gpio::pull(uint32_t pin, gpio::pull_t pull, bool on) {
    // rpi don't have that much GPIO pins...
    if (pin > 53) {
        return;
    }

    // set pull
    PUT32(GPPUD, pull);

    // compute corresponding GPIO address of requested pin
    uint32_t gpio_addr = GPIO_BASE;
    uint32_t pin_bank = (pin >> 5) << 2;
    gpio_addr += pin_bank;

    // set mask
    uint32_t old_mask = GET32(gpio_addr);
    old_mask |= (on ? 1 : 0) << pin;
    PUT32(gpio_addr, old_mask);
}

/**
 detect GPIO pin event

 @param pin     requested GPIO pin
 @param event   one of the enum `pi::gpio::gpio_event_t`
 */
void gpio::detect_event(uint32_t pin, gpio::gpio_event_t event) {
    // rpi don't have that much GPIO pins...
    if (pin > 53) {
        return;
    }

    // compute corresponding GPIO address of requested pin
    uint32_t gpio_addr = GPIO_BASE + event;
    uint32_t pin_bank = (pin >> 5) << 2;
    gpio_addr += pin_bank;

    uint32_t old_mask = GET32(gpio_addr);
    old_mask |= 1 << (pin & 31);
    PUT32(gpio_addr, old_mask);
}

/**
 detect GPIO pin level

 @param pin requested GPIO pin

 @return level
 */
bool gpio::level(uint32_t pin) {
    // rpi don't have that much GPIO pins...
    if (pin > 53) {
        return false;
    }

    // compute corresponding GPIO address of requested pin
    uint32_t gpio_addr = GPLEV0;
    uint32_t pin_bank = (pin >> 5) << 2;
    gpio_addr += pin_bank;

    return (GET32(gpio_addr) & (1 << (pin & 31))) == 0;
}
