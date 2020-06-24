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
#include "chip8.h"

#include "debug.h"

#include <cstring>

mpu::chip8::chip8(hardware_hooks const& hooks) :
    hooks(hooks)
{
    init();
}

void mpu::chip8::init(void)
{
    debug::trace("chip8::init begin");

    // system reset, clear all memory
    memset(mem, 0, sizeof(mem));
    memset(v, 0, sizeof(v));
    memset(stack, 0, sizeof(stack));
    i = 0;
    pc = 0;
    sp = 0;

    debug::trace("chip8::init completed.");
}

void mpu::chip8::hardfault(void)
{
    debug::trace("!!!chip8::hardfault invoked!!!");
    // signal hard fault

    // reset processor
    pc = 0;
    i = 0;
    sp = 0;
}

void mpu::chip8::clock(void)
{
    // fetch current instruction (big endian)
    uint16_t op;
    op = ((mem[pc] << 8u) | mem[pc + 1]);

    // decode & execute instruction
    // top level decode -> index 0-F
    switch (op & 0xf000)
    {
        case 0x0000:
            switch (op)
            {
                case 0x0000:
                    // no-op
                    break;

                case 0x00E0:
                    // clear screen
                    hooks.pDisplay->clear_screen();
                    break;

                case 0x00EE:
                    // return from call
                    if (sp == 0)
                    {
                        debug::trace("!!!chip8::clock (call) stack underflow!!!");
                        harfault();
                        return;
                    }

                    pc = stack[--sp];
                    break;

                default:
                    // sys call
                    // currently no sys-call mappings
                    // hardfault on invalid instruction!
                    debug::trace("!!!chip8::clock unknown instruction range 0x0NNN!!!");
                    hardfault();
                    return;
                    break;
            }
            break;
        case 0x1000:
            // jump to "NNN"
            pc = op & 0x0FFF;
            return;
            break;

        case 0x2000:
            // call to "NNN"
            // push this to the stack
            stack[sp++] = pc;
            if (sp >= sizeof(stack)/sizeof(*stack))
            {
                debug::trace("!!!chip8::clock (call) stack overflow!!!");
                hardfault();
                return;
            }
            break;

        case 0x3000:
            // skip over instruction if Vx == NN
            // 0x3XNN
            uint16_t regIndex = (op & 0x0F00) >> 8u;
            if (v[regIndex] == (op & 0x00FF))
            {
                pc += 2u; // skip over 1 instruction size
            }
            break;

        case 0x4000:
            // skip over if Vx != NN
            // 0x4xNN
            uint16_t regIndex = (op & 0x0F00) >> 8u;
            if (v[regIndex] != (op & 0x00FF))
            {
                pc += 2u; // skip over 1 instruction size
            }
            break;

        case 0x5000:
            // 0x5XY0
            // skip over if Vx == Vy
            uint16_t x = (op & 0x0F00) >> 8u;
            uint16_t y = (op & 0x00F0) >> 4u;

            if (op & 0x000F)
            {
                // bad instruction!
                debug::trace("!!!chip8::clock bad instruction 0x5nnX!!!");
                hadfault();
                return;
            }

            if (v[x] == v[y])
            {
                pc += 2u; // skip over 1 instruction size
            }
            break;

        case 0x6000:
            // set (0x6XNN) Vx to NN
            uint16_t x = (op & 0x0F00) >> 8u;
            uint16_t NN = (op & 0x00FF);

            v[x] = NN;
            break;

        case 0x7000:
            break;

        case 0x8000:
            break;

        case 0x9000:
            break;
        case 0xA000:
            break;
        case 0xB000:
            break;
        case 0xC000:
            break;
        case 0xD000:
            break;
        case 0xE000:
            break;
        case 0xF000:
            break;
    }

    pc += 2u;
}
