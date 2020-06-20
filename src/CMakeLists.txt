
# include paths
target_include_directories(chip8 
    PRIVATE 
        platform
)

# add hardware platform simulator
add_subdirectory(platform)

