
# include paths
target_include_directories(chip8 
    PRIVATE 
        lib/glfw/include/GLFW
)

target_sources(chip8
    PRIVATE
        platform.cpp
        lib/glfw/include/GLFW/glfw3.h
    PUBLIC
        platform.h
)

add_subdirectory(lib/glfw)
target_link_libraries(chip8 PRIVATE glfw)
