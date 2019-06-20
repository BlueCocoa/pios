//
//  kernel.cpp
//  pios
//
//  Created by Cocoa Oikawa on 2019/5/30.
//  Copyright © 2019 Cocoa Oikawa. All rights reserved.
//

#include <pi/pi.h>
#include <string.h>
#include <PiDebuggerConstant.h>

#define kCOMMAND_BUFFER_SIZE 257
char command_buffer[kCOMMAND_BUFFER_SIZE];

namespace pi {
    namespace debugger {
        template <typename T>
        void hex_to(const char * cmd, char * data) {
            for (int i = 0; i < sizeof(T) * 2; i++) {
                uint8_t value = 0;
                switch (cmd[i]) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        value = cmd[i] - '0';
                        break;
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'E':
                    case 'F':
                        value = cmd[i] - 'A' + 10;
                        break;
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                        value = cmd[i] - 'a' + 10;
                        break;
                    default:
                        break;
                }
                
                *(T *)data <<= 4;
                *data |= value;
            }
        }
        
        
        void to_hex(const void * start, uint32_t len, char * buffer) {
            
        }
        
        void parse_memory_read(const char * cmd, uint64_t * addr, uint32_t * len) {
            if (!addr || !len) {
                return;
            }
            
            hex_to<uint64_t>(cmd, (char *)addr);
            hex_to<uint32_t>(cmd + sizeof(uint64_t) * 2, (char *)len);
        }
        
        void handle_memory_read(const void * addr, uint32_t len) {
            pi::UART::write(kDebuggerMemoryRead "\n");

            uint64_t * mem = (uint64_t *)addr;
            for (uint32_t i = 0; i < len / 8; i++) {
                pi::UART::write_digit64(*mem);
                mem++;
            }
            
            pi::UART::write("\n");
        }
    }
}

extern "C"
void kernel_init(void) {
    // initilize UART
    pi::UART::init();

    // initialize graphics
    bool succeeded = pi::graphics::init(1920, 1080, 32, false);

    if (succeeded) {
        pi::graphics::console::log("[+] Cocoa!\n");
    }
    
    while (1) {
        // write something
        pi::mem::bzero(command_buffer, kCOMMAND_BUFFER_SIZE);
        uint64_t bytes = pi::UART::readn_until(command_buffer, '\n', kCOMMAND_BUFFER_SIZE - 1);
        
        pi::graphics::console::log("[+] received: ");
        pi::graphics::console::log(command_buffer);
        pi::graphics::console::log("\n");
        
        
        if (bytes == 4 && strncmp(command_buffer, kDebuggerClientHello, 4) == 0) {
            pi::graphics::console::log("[-] enter debug mode\n");
        } else if (bytes == 32) {
            if (strncmp(command_buffer, kDebuggerMemoryRead, 8) == 0) {
                uint64_t addr;
                uint32_t len;
                pi::debugger::parse_memory_read(command_buffer + 8, &addr, &len);
                
                pi::debugger::handle_memory_read((uint64_t *)addr, len);
            }
        } else {
            pi::graphics::console::log("[?] received: ");
            pi::graphics::console::log(command_buffer);
            pi::graphics::console::log("\n");
        }
    }

#ifndef __APPLE__
    asm volatile("wfi" : : : "memory");
#endif
}
