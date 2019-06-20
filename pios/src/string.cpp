//
//  string.c
//  pios
//
//  Created by Cocoa Oikawa on 19/6/2019.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <string.h>

extern "C" int strncmp(const char * s1, const char * s2, int64_t n) {
    for (uint64_t i = 0; i < n; i++) {
        if (s1[i] == '\0') {
            if (s2[i] == '\0') {
                return 0;
            } else {
                return s2[i];
            }
        } else {
            if (s2[i] == '\0') {
                return s1[i];
            } else {
                int8_t ret = s1[i] - s2[i];
                if (ret != 0) {
                    return ret;
                }
            }
        }
    }
    return 0;
}
