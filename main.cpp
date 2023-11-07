#include <iostream>
#include <cxxopts.hpp>
#include <filesystem>
#include <fstream>
#include <shadercrosscompiler.h>

int main(int argc, char** argv) {
    cxxopts::Options options{"ShaderCrossCompiler", "GLSL Shader Cross Compiler"};
    options.add_options()
            ("h,help", "Print usage.")
            ("i,input", "Input shader path", cxxopts::value<std::string>())
            ("o,output", "Input shader path", cxxopts::value<std::string>())
            ("fragment", "Compile input as a fragment shader")
            ("vertex", "Compile input as a vertex shader")
            ("geometry", "Compile input as a geometry shader")
            ("control", "Compile input as a tesselation control shader")
            ("eval", "Compile input as a tesselation evaluation shader")
            ("compute", "Compile input as a compute shader")
            ("optimize", "Enable spirv optimizer");

    cxxopts::ParseResult result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (!result.count("input")) {
        std::cout << "Missing shader file.\n\n" << options.help() << std::endl;
        return 0;
    }

    if (!result.count("output")) {
        std::cout << "Missing output.\n\n" << options.help() << std::endl;
        return 0;
    }

    std::filesystem::path shaderInputPath{ result["input"].as<std::string>() };
    std::filesystem::path shaderOutputPath{ result["output"].as<std::string>() };

    if (!std::filesystem::exists(shaderInputPath)) {
        std::cout << "Provided file doesn't exists." << std::endl;
        return 0;
    }

    std::ifstream inputFile{ shaderInputPath, std::ios_base::binary };
    if (!inputFile.is_open()) {
        std::cout << "Couldn't open provided file." << std::endl;
        return -1;
    }

    inputFile.ignore( std::numeric_limits<std::streamsize>::max() );
    size_t size = inputFile.gcount();
    inputFile.clear();
    inputFile.seekg(0, std::ios_base::beg);

    std::vector<char> data{};
    data.resize(size + 1);
    inputFile.read(data.data(), data.size());
    inputFile.close();
    data[size] = 0;

    ShaderCC::ShaderType shaderType{};

    if (result.count("fragment"))
        shaderType = ShaderCC::ShaderType::Fragment;
    if (result.count("vertex"))
        shaderType = ShaderCC::ShaderType::Vertex;
    if (result.count("geometry"))
        shaderType = ShaderCC::ShaderType::Geometry;
    if (result.count("control"))
        shaderType = ShaderCC::ShaderType::TesselationControl;
    if (result.count("eval"))
        shaderType = ShaderCC::ShaderType::TesselationEvaluation;
    if (result.count("compute"))
        shaderType = ShaderCC::ShaderType::Compute;

    ShaderCC::Shader shader{ shaderType };

    if (result.count("optimize"))
        shader.SetOptimizer();

    if(!shader.Compile(data)) {
        std::cout << "Failed Compilation." << std::endl;
        return -1;
    }

    const std::vector<char>& spirvSource{ shader.GetSpirVSource() };

    std::ofstream outputFile{ shaderOutputPath, std::ios_base::binary };
    outputFile.write(spirvSource.data(), spirvSource.size());
    outputFile.close();
}
