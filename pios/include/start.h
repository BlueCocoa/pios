//
//  start.h
//  pios
//
//  Created by Cocoa Oikawa on 2019/6/2.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#ifndef __START_H
#define __START_H

#define PAGE_SHIFT              12
#define TABLE_SHIFT             9
#define SECTION_SHIFT           (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE               (1 << PAGE_SHIFT)
#define SECTION_SIZE            (1 << SECTION_SHIFT)

#define LOW_MEMORY              (2 * SECTION_SIZE)

#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned long n);

#endif

#endif /* __START_H */
