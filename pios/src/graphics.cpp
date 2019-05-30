//
//  graphics.cpp
//  pios
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <pi/graphics.h>
#include <pi/mailbox.h>
#include <pi/mem.h>
#include <ctype.h>
#include <fonts.h>

using namespace pi;

/**
 framebuffer info
 */
volatile struct frame_buffer_info_t {
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t virtual_width;
    uint32_t virtual_height;
    uint32_t pitch;
    uint32_t depth;
    uint32_t x_offset;
    uint32_t y_offset;
    uint32_t buffer_ptr;
    uint32_t buffer_size;
} frame_buffer_info __attribute__ ((aligned (16)));

/**
 screen info
 */
volatile struct screen_info_t {
    uint32_t screen_enabled;
    uint32_t cursor_x;
    uint32_t cursor_y;
    uint32_t foreground;
    uint32_t background;
    uint32_t max_x;
    uint32_t max_y;
    uint32_t char_size_x;
    uint32_t char_size_y;
    graphics::pixel_order_t pixel_order;
    uint8_t double_frame_buffer;
    uint8_t current_frame_buffer;
} screen_info __attribute__ ((aligned (16)));

/**
 initialize graphics

 @param  width               horizontal resolution
 @param  height              vertical resolution
 @param  depth               bit depth
 @param  double_frame_buffer whether to enable double frame buffer or not

 @note
 if double frame buffer is enabled, the API calls go like below
 (1) init        when inited, the first framebuffer is displayed
 (2) draw_pixel  at this point, the pixels are changed on the 2nd framebuffer
 (3) sync        now the second framebuffer gets displayed
 (4) draw_pixel  and now the pixels are changed on the 1st framebuffer
 ...
 */
bool graphics::init(uint32_t width, uint32_t height, uint32_t depth, bool double_frame_buffer) {
    // according to the spces
    // either the horizontal or the vertical resolution cannot go beyond 4096
    if (width > 4096 || height > 4096) return false;

    // and the bit depth cannot exceed 32
    if (depth > 32) return false;

    // set corresponding value to the framebuffer struct
    frame_buffer_info.screen_width   = width;
    frame_buffer_info.screen_height  = height;
    frame_buffer_info.virtual_width  = width;
    frame_buffer_info.virtual_height = height;
    frame_buffer_info.depth = depth;

    // check if double_frame_buffer is enabled
    if (double_frame_buffer) {
        frame_buffer_info.virtual_height += height;
        screen_info.double_frame_buffer    = 1;
        screen_info.current_frame_buffer   = 2;
    } else {
        screen_info.double_frame_buffer    = 0;
        screen_info.current_frame_buffer   = 1;
    }

    // leaving these parts to the mailbox
    frame_buffer_info.x_offset = 0;
    frame_buffer_info.y_offset = 0;
    frame_buffer_info.pitch = 0;
    frame_buffer_info.buffer_ptr = 0;
    frame_buffer_info.buffer_size = 0;

    // get address of frame_buffer_info
    uint32_t request = (uint32_t)((uint64_t)&frame_buffer_info);

    // send framebuffer allocation request to mailbox
    mailbox::write(1, request);

    // wait for mailbox handle the request
    uint32_t ret = 0xFF;
    do {
        ret = mailbox::read(1);
    } while (ret != 0);

    // and sometimes, bad things happen
    if (frame_buffer_info.buffer_ptr == 0) return false;
    if (frame_buffer_info.pitch == 0) return false;

#warning NO MMU assumed
    // convert to physical address
    frame_buffer_info.buffer_ptr = frame_buffer_info.buffer_ptr - 0x40000000;

    // set corresponding screen info
    screen_info.char_size_x = 6;
    screen_info.char_size_y = 10;
    screen_info.screen_enabled = true;
    screen_info.cursor_x = 0;
    screen_info.cursor_y = 0;
    screen_info.background = 0x00000000;
    screen_info.max_x = width / screen_info.char_size_x;
    screen_info.max_y = height / screen_info.char_size_y;
    screen_info.pixel_order = detect_pixel_order();

    // set foreground color based on color depth
    if (frame_buffer_info.depth == 16) {
        screen_info.foreground = 0xFFFF;
    } else if (frame_buffer_info.depth == 24) {
        screen_info.foreground = 0xFFFFFFFF;
    } else if (frame_buffer_info.depth == 32) {
        screen_info.foreground = 0xFFFFFF00;

        // enable alpha mode in mailbox
        uint32_t param[1] = { 0 };
        mailbox::property(mailbox::ALPHA_MODE_SET, param);
    }

    return true;
}

/**
 get current pixel order

 @return one of `pixel_order_t::BGR`, `pixel_order_t::RGB`, `pixel_order_t::ERROR_WHILE_GET`
 */
graphics::pixel_order_t graphics::pixel_order() {
    return screen_info.pixel_order;
}

/**
 auto detect the correct pixel order for the display

 @return one of `pixel_order_t::BGR`, `pixel_order_t::RGB`, `pixel_order_t::ERROR_WHILE_GET`
 */
graphics::pixel_order_t graphics::detect_pixel_order() {
    volatile uint32_t * result = mailbox::property(mailbox::PIXEL_ORDER_GET);
    if (result[0] != 0) {
        if (result[1] == 0x1) {
            return RGB;
        } else if (result[1] == 0x0) {
            return BGR;
        }
    }
    return ERROR_WHILE_GET;
}

/**
 manually set the pixel order

 @param order either `pixel_order_t::BGR` or `pixel_order_t::RGB`
 */
void graphics::set_pixel_order(graphics::pixel_order_t order) {
    screen_info.pixel_order = order;
}

#pragma mark - graphics::console

/**
 color map for console
 */
static uint32_t colorMap[][8] = {
    {
        0,0,0,0,0,0,0,0
    },
    {
        0b1111100000000000,
        0b0000011111100000,
        0b0000000000011111,
        0b1111111111100000,
        0b1111100000011111,
        0b0000011111111111,
        0b1111111111111111,
        0b0000000000000000
    },
    {
        0xFF0000,
        0x00FF00,
        0x0000FF,
        0xFFFF00,
        0xFF00FF,
        0x00FFFF,
        0xFFFFFF,
        0x000000
    },
    {
        0xFF000000,
        0x00FF0000,
        0xAAF5FF00,
        0xFFFF0000,
        0xFF00FF00,
        0x00FFFF00,
        0xFFFFFF00,
        0x00000000
    }
};

/**
 log a single character

 @param c character
 */
void graphics::console::logc(char c) {
    uint32_t buffer_offset = 0;
    // if double frame buffer enabled
    if (screen_info.double_frame_buffer) {
        // if we are in the 2nd framebuffer
        if (screen_info.current_frame_buffer == 2) {
            // compute corresponding buffer offset
            buffer_offset += (frame_buffer_info.screen_height * frame_buffer_info.pitch);
        }
    }

    uint32_t row, addr;
    int col;

    // test printable
    c = isprint(c);

    // if c is printable
    if (c) {
        // compute the index of printable character
        c -= 32;
    }

    if (frame_buffer_info.depth == 16) {
        // there are 3 compoents in both foreground and background
        uint8_t fR, fG, fB;
        uint8_t bR, bG, bB;

        // compute foreground and background
        fR = (screen_info.foreground & 0xF800) >> 11;
        fG = (screen_info.foreground & 0x07E0) >> 5;
        fB = (screen_info.foreground & 0x001F);

        bR = (screen_info.background & 0xF800) >> 11;
        bG = (screen_info.background & 0x07E0) >> 5;
        bB = (screen_info.background & 0x001F);

        // adjust for given pixel order
        uint16_t fColor, bColor;
        if (screen_info.pixel_order == graphics::BGR) {
            fColor = ((fB << 11) | (fG << 5) | fR);
            bColor = ((fB << 11) | (fG << 5) | fR);
        } else {
            fColor = ((fR << 11) | (fG << 5) | fB);
            bColor = ((fR << 11) | (fG << 5) | fB);
        }

        // draw character
        for (row = 0; row < screen_info.char_size_y; row++) {
            addr = (row+screen_info.cursor_y * screen_info.char_size_y) * frame_buffer_info.pitch + screen_info.cursor_x * screen_info.char_size_x * 2;
            for(col = (screen_info.char_size_x - 2); col >= 0; col--) {
                if (row < (screen_info.char_size_y - 1) && (fonts[c][row] & (1 << col))){
                    *(uint16_t *)(frame_buffer_info.buffer_ptr + addr + buffer_offset) = fColor;
                } else {
                    *(uint16_t *)(frame_buffer_info.buffer_ptr + addr + buffer_offset) = bColor;
                }
                addr += 2;
            }
            *(uint16_t *)(frame_buffer_info.buffer_ptr + addr + buffer_offset) = bColor;
        }
    } else if (frame_buffer_info.depth == 24) {
        // there are 3 compoents in both foreground and background
        __attribute__ ((aligned (8)))
        struct _color_t {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        } fColor, bColor;

        // compute foreground and background
        fColor.r = (screen_info.foreground & 0xFF0000) >> 16;
        fColor.g = (screen_info.foreground & 0x00FF00) >> 8;
        fColor.b = (screen_info.foreground & 0x0000FF);

        bColor.r = (screen_info.background & 0xFF0000) >> 16;
        bColor.g = (screen_info.background & 0x00FF00) >> 8;
        bColor.b = (screen_info.background & 0x0000FF);

        // adjust for given pixel order
        if (screen_info.pixel_order == graphics::BGR) {
            fColor.r ^= fColor.b ^= fColor.r ^= fColor.b;
            bColor.r ^= bColor.b ^= bColor.r ^= bColor.b;
        }

        // draw character
        for (row = 0; row < screen_info.char_size_y; row++) {
            addr = (row + screen_info.cursor_y * screen_info.char_size_y) * frame_buffer_info.pitch + screen_info.cursor_x * screen_info.char_size_x * 3;
            for(col = (screen_info.char_size_x - 2); col >= 0; col--) {
                if (row < (screen_info.char_size_y - 1) && (fonts[c][row] & (1 << col))) {
                    *(_color_t*)(frame_buffer_info.buffer_ptr + addr + buffer_offset) = fColor;
                } else {
                    *(_color_t*)(frame_buffer_info.buffer_ptr + addr + buffer_offset) = bColor;
                }
                addr += 3;
            }
            *(_color_t*)(frame_buffer_info.buffer_ptr + addr + buffer_offset) = bColor;
        }
    } else if (frame_buffer_info.depth == 32) {
        // there are 4 compoents in both foreground and background
        uint8_t fA, fR, fG, fB;
        uint8_t bA, bR, bG, bB;

        // compute foreground and background
        fR = (screen_info.foreground & 0xFF000000) >> 24;
        fG = (screen_info.foreground & 0x00FF0000) >> 16;
        fB = (screen_info.foreground & 0x0000FF00) >> 8;
        fA = (screen_info.foreground & 0x000000FF);

        bR = (screen_info.background & 0xFF000000) >> 24;
        bG = (screen_info.background & 0x00FF0000) >> 16;
        bB = (screen_info.background & 0x0000FF00) >> 8;
        bA = (screen_info.background & 0x000000FF);

        // adjust for given pixel order
        __attribute__ ((aligned (32)))
        struct _color_t {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        } fColor = {
            fR,
            fG,
            fB,
            fA
        }, bColor = {
            bR,
            bG,
            bB,
            bA
        };
        if (screen_info.pixel_order == graphics::BGR) {
            fColor.r ^= fColor.b ^= fColor.r ^= fColor.b;
            bColor.r ^= bColor.b ^= bColor.r ^= bColor.b;
        }

        // draw character
        for (row = 0; row < screen_info.char_size_y; row++) {
            addr = (row + screen_info.cursor_y * screen_info.char_size_y) * frame_buffer_info.pitch + screen_info.cursor_x * screen_info.char_size_x * 4;
            for (col = (screen_info.char_size_x - 2); col >= 0; col--) {
                if (row < (screen_info.char_size_y - 1) && (fonts[c][row] & (1 << col))){
                    *(_color_t*)(frame_buffer_info.buffer_ptr + addr + buffer_offset) = fColor;
                } else {
                    *(_color_t*)(frame_buffer_info.buffer_ptr + addr + buffer_offset) = bColor;
                }
                addr += 4;
            }
            *(_color_t*)(frame_buffer_info.buffer_ptr + addr + buffer_offset) = bColor;
        }
    }

    if (++screen_info.cursor_x >= screen_info.max_x) {
        line_feed();
    }
}

/**
 log a string

 @param log string
 */
void graphics::console::log(const char * log) {
    // A small stack to allow temporary colour changes in text
    static uint64_t colour_stack[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    static uint32_t colour_sp = 8;

    unsigned char ch;
    while ((ch = (unsigned char)*log)) {
        log++;

        // Deal with control codes
        uint8_t color_map_entry = (frame_buffer_info.depth / 8) - 1;
        switch (ch) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8: {
                screen_info.foreground = colorMap[color_map_entry][ch - 1];
                continue;
            }
            case 9: {
                // foreground half brightness
                screen_info.foreground = (screen_info.foreground >> 1) & 0b0111101111101111;
                continue;
            }
            case 10: {
                line_feed();
                continue;
            }
            case 11: {
                // push color stack
                if (colour_sp) {
                    colour_sp--;
                }
                colour_stack[colour_sp] = screen_info.background;
                colour_stack[colour_sp] <<= 32;
                colour_stack[colour_sp] = screen_info.foreground;
                continue;
            }
            case 12: {
                // pop color stack
                screen_info.foreground = colour_stack[colour_sp] & 0xFFFFFFFF;
                screen_info.background = colour_stack[colour_sp] >> 32;
                if (colour_sp < 8) {
                    colour_sp++;
                }
                continue;
            }
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24: {
                screen_info.background = colorMap[color_map_entry][ch - 17];
                continue;
            }
            case 25: {
                // background half brightness
                screen_info.background = (screen_info.background >> 1) & 0b0111101111101111;
                continue;
            }
        }
        logc(ch);
    }
}

/**
 line feed
 */
void graphics::console::line_feed() {
    uint32_t source;

    // compute bytes per row
    register uint32_t bytes_per_row = screen_info.char_size_y * frame_buffer_info.pitch;

    // set cursor's x to 0
    screen_info.cursor_x = 0;

    // if we didn't exceed the screen's height
    if (screen_info.cursor_y < (screen_info.max_y - 1)) {
        // just add 1 to the y of cursor
        screen_info.cursor_y++;
        return;
    }

    // otherwise
    uint32_t buffer_offset = 0;

    // if double frame buffer enabled
    if (screen_info.double_frame_buffer) {
        // if we are in the 2nd framebuffer
        if (screen_info.current_frame_buffer == 2) {
            // compute corresponding buffer offset
            buffer_offset += (frame_buffer_info.screen_height * frame_buffer_info.pitch);
        }
    }

    source = frame_buffer_info.buffer_ptr + bytes_per_row + buffer_offset;

    mem::memmove((void *)(uint64_t)frame_buffer_info.buffer_ptr, (void *)(uint64_t)source, (screen_info.max_y - 1) * bytes_per_row);
    mem::bzero((void *)(uint64_t)(frame_buffer_info.buffer_ptr + (screen_info.max_y - 1) * bytes_per_row), bytes_per_row);
}

