// MIT License
// 
// Copyright (c) 2020 Jimi Huard
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <cstdint>

#include "debug.h"

namespace mpu
{
    struct display_hook
    {
        virtual void clear_screen(void) = 0;
    };

    struct input_hook
    {
        enum key
        {
            // hex keyboard
            left = 2,
            right = 4,
            up = 6,
            down = 8,
        };
        virtual void get_keystate(key code) = 0;
    };

    struct hardware_hooks
    {
        display_hook* pDisplay;
        input_hook* pInput;
    };

    class chip8 : public debug::isanity_testable
    {
        public:
            const static uint32_t memory_size = 4096u;
            const static uint32_t stack_depth = 16;
            enum reg
            {
                v0 = 0,
                v1,v2,v3,v4,v5,v6,v7,v8,v9,
                vA,vB,vC,vD,vE,
                vF,// flag register (carry, no borrow, etc)
                num
            };

            chip8(hardware_hooks const& hooks);

            void init(void);
            void clock(void);
            void hardfault(void);

            virtual void test_sanity(void);

        private:
            uint8_t  mem[memory_size];
            uint8_t  v[reg::num];
            uint16_t i;
            uint16_t pc;
            uint16_t stack[stack_depth];
            uint16_t sp;
            hardware_hooks hooks;
    };
}

#endif//__CHIP8_H__
