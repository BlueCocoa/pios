//
//  ctype.h
//  pios
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#ifndef __CTYPE_H
#define __CTYPE_H

/**
 printing character test (space character inclusive)

 @param c char to test

 @return zero if the character tests false and returns non-zero if the character tests
 true
 */
extern "C" int isprint(int c);

#endif /* __CTYPE_H */
