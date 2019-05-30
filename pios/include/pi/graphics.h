//
//  graphics.h
//  pios
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#ifndef __PI_GRAPHICS_H
#define __PI_GRAPHICS_H

#include <sys/types.h>

namespace pi {

namespace graphics {
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
    bool init(uint32_t width, uint32_t height, uint32_t depth, bool double_frame_buffer);

    /**
     pixel order

     @note
     Raspberry Pi supports both BGR and RGB order
     And it could auto detect the order from HDMI
     However, in some rare cases it may not work well
     */
    enum pixel_order_t {
        BGR = 0x0,
        RGB = 0x1,
        ERROR_WHILE_GET = 0x2
    };

    /**
     get current pixel order

     @return one of `pixel_order_t::BGR`, `pixel_order_t::RGB`, `pixel_order_t::ERROR_WHILE_GET`
     */
    pixel_order_t pixel_order();

    /**
     auto detect the correct pixel order for the display

     @return one of `pixel_order_t::BGR`, `pixel_order_t::RGB`, `pixel_order_t::ERROR_WHILE_GET`
     */
    pixel_order_t detect_pixel_order();

    /**
     manually set the pixel order

     @param order either `pixel_order_t::BGR` or `pixel_order_t::RGB`
     */
    void set_pixel_order(pixel_order_t order);
} // graphics

} // pi

namespace pi {

namespace graphics {

namespace console {
    /**
     log a single character

     @param c character
     */
    void logc(char c);

    /**
     log a string

     @param log string
     */
    void log(const char * log);

    /**
     line feed
     */
    void line_feed();
} // console

} // graphics

} // pi

#endif /* __PI_GRAPHICS_H */
