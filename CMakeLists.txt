cmake_minimum_required(VERSION 3.26)
project(ShaderCrossCompiler)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(3rdparty/cxxopts)
add_subdirectory(3rdparty/glslang)

add_executable(ShaderCrossCompiler main.cpp
        tokenizer.h)
target_link_libraries(ShaderCrossCompiler cxxopts glslang)