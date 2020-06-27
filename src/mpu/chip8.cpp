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
                        hardfault();
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
        {
            // skip over instruction if Vx == NN
            // 0x3XNN
            uint16_t regIndex = (op & 0x0F00) >> 8u;
            if (v[regIndex] == (op & 0x00FF))
            {
                pc += 2u; // skip over 1 instruction size
            }
        }break;

        case 0x4000:
        {
            // skip over if Vx != NN
            // 0x4xNN
            uint16_t regIndex = (op & 0x0F00) >> 8u;
            if (v[regIndex] != (op & 0x00FF))
            {
                pc += 2u; // skip over 1 instruction size
            }
        } break;

        case 0x5000:
        {
            // 0x5XY0
            // skip over if Vx == Vy
            uint16_t x = (op & 0x0F00) >> 8u;
            uint16_t y = (op & 0x00F0) >> 4u;

            if (op & 0x000F)
            {
                // bad instruction!
                debug::trace("!!!chip8::clock bad instruction 0x5nnX!!!");
                hardfault();
                return;
            }

            if (v[x] == v[y])
            {
                pc += 2u; // skip over 1 instruction size
            }
        } break;

        case 0x6000:
        {
            // set (0x6XNN) Vx to NN
            uint16_t x = (op & 0x0F00) >> 8u;
            uint16_t NN = (op & 0x00FF);

            v[x] = NN;
        } break;

        case 0x7000:
        {
            // 0x7XNN
            // Vx += NN
            uint16_t x = (op & 0x0F00) >> 8u;
            uint16_t NN = (op & 0x00FF);
            
            v[x] += NN;
        } break;

        case 0x8000:
        {
            // 0x8 XY N
            uint16_t x = (op & 0x0F00) >> 8;
            uint16_t y = (op & 0x00F0) >> 4;
            switch (op & 0x000F)//N
            {
                case 0://==N
                    v[x] = v[y];
                    break;

                case 1://==N
                    v[x] |= v[y];
                    break;

                case 2://==N
                    v[x] &= v[y];
                    break;

                case 3://==N
                    v[x] ^= v[y];
                    break;

                case 4://==N
                {
                    uint16_t result = v[x] + v[y];
                    v[vF] = (result > 0xFF);
                    v[x] = result & 0xFF;
                } break;

                case 5://==N
                    v[vF] = (v[x] > v[y]);
                    v[x] -= v[y];
                    break;

                case 6://==N
                    v[vF] = v[x] & 0x1;
                    v[x] >>= 1;
                    break;

                case 7://==N
                    v[vF] = (v[y] > v[x]);
                    v[x] = v[y] - v[x];
                    break;

                case 0xE://==N
                    v[vF] = (v[x] & 0x80) >> 7u;
                    v[x] <<= 1;
                    break;

                default:
                    debug::trace("!!!chip8::clock badly formed instruction about M: 0x8xxM!!!");
                    hardfault();
                    return;
            }
        } break;

        case 0x9000:
        {
            // 0x9xy0
            if (op & 0xF)
            {
                debug::trace("!!!chip8::clock badly formed instruction about M: 0x9xxM!!!");
            }

            uint16_t x = (op & 0x0F00) >> 8;
            uint16_t y = (op & 0x00F0) >> 4;

            if (v[x] == v[y])
            {
                pc += 2; // skip next instruction
            }
        } break;

        case 0xA000:
            i = op & 0x0FFF;
            break;

        case 0xB000:
            pc = v[v0] + (op & 0x0FFF);
            break;

        case 0xC000:
        {
            uint16_t x  = (op & 0x0F00) >> 8;
            uint16_t NN = op & 0x00FF;
            v[x] = (rand() & NN);
        } break;

        case 0xD000:
            // 0xDxyN
            // draw(vx, vy, N)
            break;

        case 0xE000:
            if ((op & 0xF0FF) == 0xE09E)
            {
                uint16_t x = (op & 0x0F00) >> 8;
                // if key( v[x] ) is pressed, skip
            }
            else if ((op & 0xF0FF) == 0xE0A1)
            {
                uint16_t x = (op & 0x0F00) >> 8;
                // if key( v[x] ) is released, skip
            }
            else
            {
                debug::trace("!!!chip8::clock bad instruction!!!");
                hardfault();
            }
            break;

        case 0xF000:
        {
            uint16_t x = (op & 0x0F00) >> 8;
            switch (op & 0xF0FF)
            {
                case 0xF007:
                    // Vx = get_delay()
                    break;

                case 0xF00A:
                    // Vx = get_key() (blocking)
                    break;

                case 0xF015:
                    // delay_timer(Vx)
                    break;

                case 0xF018:
                    // sound_timer(Vx)
                    break;

                case 0xF01E:
                    // I += Vx
                    i += v[x];
                    break;

                case 0xF029:
                    // I = sprite_addr[Vx]
                    break;

                case 0xF033:
                    // set_bcd(Vx)
                    // I[0] = BCD(3); MSB (100s) 
                    // I[1] = BCD(2);     (10s)
                    // I[2] = BCD(1); LSB (1s)
                    // take BCD rep of Vx, place into I[...]
                    mem[i] = v[x] / 100;
                    mem[i + 1] = (v[x] - mem[i]) / 10;
                    mem[i + 2] = v[x] - mem[i + 1]*10 - mem[i]*100;
                    break;

                case 0xF055:
                    // reg_dump(V[0:x] -> I) 
                    // otherwise known as push registers to location in I (I is not modified)
                    if (i + x > 0xFFF)
                    {
                        debug::trace("!!!chip8::clock overflow in I at 0xFx55!!!");
                        hardfault();
                        return;
                    }

                    for (uint32_t j = 0; j < x; ++j)
                    {
                        mem[i + j] = v[j];
                    }
                    break;

                case 0xF065:
                    // reg_load(I -> V[0:x])
                    // otherwise known as pop registers from location in I (I is not modified)
                    if (i + x > 0xFFF)
                    {
                        debug::trace("!!!chip8::clock overflow in I at 0xFx65!!!");
                        hardfault();
                        return;
                    }

                    for (uint32_t j = 0; j < x; ++j)
                    {
                        v[j] = mem[i + j];
                    }
                    break;

                default:
                    debug::trace("!!!chip8::clock bad instruction around 0xFxMM!!!");
                    hardfault();
                    return;
                    break;
            }
        } break;

        default:
            debug::trace("!!!chip8::clock bad instruction!!!");
            hardfault();
            return;
            break;
    }

    pc += 2u;
}
