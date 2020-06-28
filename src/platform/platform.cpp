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

void platform::platform::initialize(display_descriptor const &init)
{
    descriptor = init;

    static bool s_initOnce = false;

    if (s_initOnce == false)
    {
        if (!glfwInit())
        {
            debug::trace("platform::initialize glfwInit() failed!");
            return;
        }

        s_initOnce = true;
    }

    windowHandle = static_cast<void*>(glfwCreateWindow(init.pixel_width(), 
                                                       init.pixel_height(), 
                                                       init.title.c_str(), 
                                                       NULL, 
                                                       NULL));
    if (windowHandle == nullptr)
    {
        glfwTerminate();
        debug::trace("platform::initialize glfwCreateWindow() failed!");
        return;
    }

    glfwMakeContextCurrent(static_cast<GLFWwindow*>(windowHandle));

    pixelBuffer.clear();
    pixelBuffer.resize(init.width); // pixelBuffer[x][y]
    for (auto& row : pixelBuffer) // for each row, add a column
        row.resize(init.height);

    // initialize to noise
    for (int i = 0; i < init.width; ++i)
        for (int j = 0; j < init.height; ++j)
            pixelBuffer[i][j] = (rand() % 2) == 0 ?
                                    init.bg_color : init.fg_color;

    debug::trace("platform::initialize completed.");
}

bool platform::platform::ui_close(void) 
{
    return glfwWindowShouldClose(static_cast<GLFWwindow *>(windowHandle));
}

void platform::platform::update(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glViewport(0, 0, descriptor.pixel_width(), descriptor.pixel_height());
    
    uint8_t *pixelData = new uint8_t[descriptor.pixel_width() * descriptor.pixel_height()];

    for (int x = 0; x < descriptor.pixel_width(); ++x)
    {
        for (int y = 0; y < descriptor.pixel_height(); ++y)
        {
            pixelData[x + y * descriptor.pixel_width()] = pixelBuffer[x / descriptor.pixel_size][y / descriptor.pixel_size];
        }
    }

    glDrawPixels(descriptor.pixel_width(),
                 descriptor.pixel_height(),
                 GL_RGB,
                 GL_UNSIGNED_BYTE_3_3_2,
                 pixelData);
    
    delete[] pixelData;

    glfwSwapBuffers(static_cast<GLFWwindow*>(windowHandle));

    glfwPollEvents();
}

void platform::platform::set_pixel(int32_t x, int32_t y)
{
    pixel(x, y, descriptor.fg_color);
}

void platform::platform::clear_pixel(int32_t x, int32_t y)
{
    pixel(x, y, descriptor.bg_color);
}

void platform::platform::pixel(int32_t x, int32_t y, uint8_t rgb)
{
    pixelBuffer[x][y] = rgb;
}

void platform::platform::clear_screen(void)
{
    for (int x = 0; x < descriptor.width; ++x)
        for (int y = 0; y < descriptor.height; ++y)
            pixelBuffer[x][y] = descriptor.bg_color;
}

void platform::platform::test_sanity(void)
{
    debug::trace("platform::test_sanity begin");

    debug::trace("platform::test_sanity::test_window begin");
    test_window();
    debug::trace("platform::test_sanity::test_window completed.");

    debug::trace("platform::test_sanity completed.");
}

void platform::platform::test_window(void)
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
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
}
