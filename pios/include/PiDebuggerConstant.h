//
//  PiDebuggerConstant.h
//  pios-debugger
//
//  Created by Cocoa Oikawa on 2019/6/19.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#ifndef PiDebuggerConstant_h
#define PiDebuggerConstant_h

#define kDebuggerClientHello  "pidb"

/// pidbrmemXXXXXXXXXXXXXXXXCCCCCCCC
/// XXXXXXXXXXXXXXXX is the start address
/// CCCCCCCC is #bytes to be read
#define kDebuggerMemoryRead   "pidbrmem"

/// pidbwmemXXXXXXXXXXXXXXXXCCCCCCCCDDDD...DDDD
/// XXXXXXXXXXXXXXXX is the start address
/// CCCCCCCC is #bytes to be written
/// DDDD....DDDD is the data to be written
#define kDebuggerMemoryWrite  "pidbwmem"

#endif /* PiDebuggerConstant_h */
