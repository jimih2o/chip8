
# include paths
target_include_directories(chip8 
    PRIVATE 
        # debug must come first!
        debug
        mpu
        platform
)

# add hardware platform simulator
# debug must come first!
add_subdirectory(debug)
add_subdirectory(mpu)
add_subdirectory(platform)

