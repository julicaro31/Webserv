cmake_minimum_required(VERSION 3.8)
project(my_project)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Change path from /src if needed, or add more directories
file(GLOB_RECURSE sources
        "${CMAKE_SOURCE_DIR}/src/*.c"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        )
# Add precompiler definitions like that:
add_definitions(-DSOME_DEFINITION)

add_executable(my_app ${sources})

# Add more include directories if needed
target_include_directories(my_app PUBLIC "${CMAKE_SOURCE_DIR}/include")

# If you have precompiled headers you can add them like this
# target_precompiled_headers(my_app PRIVATE "${CMAKE_SOURCE_DIR}/src/pch.h")
