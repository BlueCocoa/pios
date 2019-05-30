//
//  ctype.c
//  pios
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <ctype.h>

/**
 printing character test (space character inclusive)

 @param c char to test

 @return zero if the character tests false and returns non-zero if the character tests
 true
 */
extern "C" int isprint(int c) {
    if (c >= 32 && c <= 127) {
        return c;
    } else {
        return 0;
    }
}
