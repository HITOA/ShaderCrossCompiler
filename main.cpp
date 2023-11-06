#include <iostream>
#include <cxxopts.hpp>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <filesystem>

int main(int argc, char** argv) {
    cxxopts::Options options{"ShaderCrossCompiler", "GLSL Shader Cross Compiler"};
    options.add_options()
            ("h,help", "Print usage.", cxxopts::value<std::string>())
            ("i,input", "Input shader path");

    cxxopts::ParseResult result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (!result.count("input")) {
        std::cout << "Missing shader file.\n\n" << options.help() << std::endl;
        return 0;
    }

    std::filesystem::path shaderPath{};

    if (result.count("input"))
        shaderPath = std::filesystem::path{ result["input"].as<std::string>() };

    if (!std::filesystem::exists(shaderPath)) {
        std::cout << "Provided file doesn't exists." << std::endl;
        return 0;
    }

}
