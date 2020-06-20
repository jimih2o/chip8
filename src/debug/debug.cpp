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
#include "debug.h"

#include <atomic>
#include <iostream>
#include <list>
#include <mutex>
#include <algorithm>

static std::atomic<bool> s_debugEnabled(false);
static std::mutex s_sanityListMutex;
static std::list<debug::isanity_testable*> s_sanityList;

debug::isanity_testable::isanity_testable()
{
    s_sanityListMutex.lock();
    s_sanityList.push_back(this);
    s_sanityListMutex.unlock();
}

debug::isanity_testable::~isanity_testable()
{
    s_sanityListMutex.lock();
    std::remove(s_sanityList.begin(), s_sanityList.end(), this);
    s_sanityListMutex.unlock();
}

void debug::enable(void)
{
    s_debugEnabled = true;
}

void debug::disable(void)
{
    s_debugEnabled = false;
}

void debug::trace(std::string const &message)
{
    if (s_debugEnabled)
    {
        std::cout << "tr>> " << message << std::endl;
    }
}

void debug::test_sanity(void)
{
    for (auto object : s_sanityList)
    {
        object->test_sanity();
    }
}
