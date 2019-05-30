//
//  extern.h
//  pios-mac
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#ifndef __PI_EXTERN_H
#define __PI_EXTERN_H

/**
 wait one CPU tick
 */
extern "C" void wait_op(void);

/**
 wait given CPU ticks
 */
extern "C" void delay(unsigned int ticks);

#endif /* __PI_EXTERN_H */
