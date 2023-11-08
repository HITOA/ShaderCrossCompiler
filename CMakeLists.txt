cmake_minimum_required(VERSION 3.26)
project(ShaderCrossCompiler)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(3rdparty/cxxopts)
add_subdirectory(3rdparty/glslang)
add_subdirectory(3rdparty/SPIRV-Cross)

add_library(ShaderCrossCompilerLib library/tokenizer.h
        library/tokenizer.cpp
        library/shadercrosscompiler.h
        library/shadercrosscompiler.cpp)
target_link_libraries(ShaderCrossCompilerLib glslang glslang-default-resource-limits SPIRV spirv-cross-glsl spirv-cross-hlsl spirv-cross-msl)
target_include_directories(ShaderCrossCompilerLib PUBLIC library)

add_executable(ShaderCrossCompiler main.cpp)
target_link_libraries(ShaderCrossCompiler ShaderCrossCompilerLib cxxopts)