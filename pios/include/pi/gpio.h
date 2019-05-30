//
//  gpio.h
//  pios-mac
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#ifndef __PI_GPIO_H
#define __PI_GPIO_H

#include <sys/types.h>


#define GPIO_BASE       (ARM_PERIPHERALS_BASE + 0x200000)

#define GPFSEL0         (GPIO_BASE + 0x00)
#define GPFSEL1         (GPIO_BASE + 0x04)
#define GPFSEL2         (GPIO_BASE + 0x08)
#define GPFSEL3         (GPIO_BASE + 0x0C)
#define GPFSEL4         (GPIO_BASE + 0x10)
#define GPFSEL5         (GPIO_BASE + 0x14)

#define GPSET0          (GPIO_BASE + 0x1C)
#define GPSET1          (GPIO_BASE + 0x20)

#define GPCLR0          (GPIO_BASE + 0x28)
#define GPCLR1          (GPIO_BASE + 0x2C)

#define GPLEV0          (GPIO_BASE + 0x34)
#define GPLEV1          (GPIO_BASE + 0x38)

#define GPEDS0          (GPIO_BASE + 0x40)
#define GPEDS1          (GPIO_BASE + 0x44)

#define GPREN0          (GPIO_BASE + 0x4C)
#define GPREN1          (GPIO_BASE + 0x50)

#define GPFEN0          (GPIO_BASE + 0x58)
#define GPFEN1          (GPIO_BASE + 0x5C)

#define GPHEN0          (GPIO_BASE + 0x64)
#define GPHEN1          (GPIO_BASE + 0x68)

#define GPLEN0          (GPIO_BASE + 0x70)
#define GPLEN1          (GPIO_BASE + 0x74)

#define GPAREN0         (GPIO_BASE + 0x7C)
#define GPAREN1         (GPIO_BASE + 0x80)

#define GPAFEN0         (GPIO_BASE + 0x88)
#define GPAFEN1         (GPIO_BASE + 0x8C)

#define GPPUD           (GPIO_BASE + 0x94)
#define GPPUDCLK0       (GPIO_BASE + 0x98)
#define GPPUDCLK1       (GPIO_BASE + 0x9C)

// GPIO pins used for EMMC.
#define GPIO_DAT3       53
#define GPIO_DAT2       52
#define GPIO_DAT1       51
#define GPIO_DAT0       50
#define GPIO_CMD        49
#define GPIO_CLK        48
#define GPIO_CD         47

#endif /* __PI_GPIO_H */
