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
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <cstdint>
#include <string>
#include <vector>

#include "debug.h"
#include "chip8.h"

namespace platform
{
    int32_t const     default_pixel_size     = 10;
    int32_t const     default_display_width  = 64;
    int32_t const     default_display_height = 48;
    std::string const default_display_title  = "Chip-8";
    uint8_t const     default_bg_color       = 0; // 3 3 2 format
    uint8_t const     default_fg_color       = 255;


    struct display_descriptor
    {
        int32_t pixel_size = default_pixel_size;
        int32_t width      = default_display_width;
        int32_t height     = default_display_height;
        std::string title  = default_display_title;
        float bg_color     = default_bg_color;
        float fg_color     = default_fg_color;

        int32_t pixel_width() const {return width * pixel_size;}
        int32_t pixel_height() const{return height* pixel_size;}
    };

    class display : public debug::isanity_testable, public mpu::display_hook
    {
        public:
            void initialize(display_descriptor const &init = display_descriptor());
            void display_splash(void);
            bool ui_close(void);
            void update(void);

            void set_pixel(int32_t x, int32_t y);
            void clear_pixel(int32_t x, int32_t y);
            void pixel(int32_t x, int32_t y, uint8_t rgb);
            
            virtual void clear_screen(void);
            virtual void test_sanity(void);
        private:
            display_descriptor                descriptor;
            void                             *windowHandle;
            std::vector<std::vector<uint8_t>> pixelBuffer;

            void test_window(void);
    };
}

#endif//__PLATFORM_H__
