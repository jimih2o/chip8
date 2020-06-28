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

#include "platform.h"
#include "debug.h"

#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

namespace std {
std::string& toupper(std::string &str) 
{
    std::transform(str.begin(), str.end(), str.begin(),
        [](char c){return std::toupper(c);});
    return str;
}
}

static int parse_command_line(int argc, char **argv);

struct
{
    bool runSanityTest = false;
} s_config;

int main(int argc, char **argv)
{
    int cmdLine = parse_command_line(argc - 1, &argv[1]);

    platform::platform platform;
    
    mpu::hardware_hooks hooks = 
    {
        .pDisplay = &platform,
    };
    mpu::chip8 chip8(hooks);

    if (s_config.runSanityTest)
    {
        std::cout << "Begin Sanity Test\n";
        debug::test_sanity();
        std::cout << "Sanity Test Complete.\n";
    }
    else
    {
        platform.initialize();

        while (platform.ui_close() == false)
        {
            platform.update();
        }
    }

    return cmdLine;
}

static int parse_command_line(int argc, char **argv)
{
    if (argc < 1) return 0; // nothing to do

    for (int i = 0; i < argc; ++i)
    {
        std::string opt(argv[i]);

        // debug flag
        if (opt == "d" ||
            opt == "-d" ||
            opt == "-D" ||
            std::toupper(opt) == "-DEBUG")
        {
            debug::enable();
        }
        else if (opt == "s" ||
                 opt == "-s" ||
                 opt == "-S" ||
                 std::toupper(opt) == "-SANITY")
        {
            debug::enable();
            s_config.runSanityTest = true;
        }
        else
        {
            std::cout << "Unrecognized option: \"" << opt << "\"" << std::endl;
        }
    }

    return 0;
}