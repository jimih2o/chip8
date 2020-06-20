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

#include "glfw3.h"

void platform::display::initialize(display_descriptor const &init)
{
    descriptor = init;

    // todo validate descriptor

    debug::trace("platform::display::initialize completed.");
}

void platform::display::test_sanity(void)
{
    debug::trace("platform::display::test_sanity begin");

    debug::trace("platform::display::test_sanity::test_window begin");
    test_window();
    debug::trace("platform::display::test_sanity::test_window completed.");

    debug::trace("platform::display::test_sanity completed.");
}

void platform::display::test_window(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {
        debug::trace("test_window -- glfwInit failed!");
        return;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        debug::trace("test_window -- glfwCreateWindow failed!");
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    for (int i = 0; i < 60 && !glfwWindowShouldClose(window); ++i)
    {
        /* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
}
