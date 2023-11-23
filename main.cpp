#include <iostream>
#include <cxxopts.hpp>
#include <filesystem>
#include <fstream>
#include <shadercrosscompiler.h>

#define SHADERCC_IMPL_AGGREGATE_WRITE
#include <aggregateshader.h>
#include <shaderinfo.h>

int main(int argc, char** argv) {
    cxxopts::Options options{"ShaderCrossCompiler", "GLSL Shader Cross Compiler"};
    options.add_options()
            ("h,help", "Print usage.")
            ("i,input", "Input shader path", cxxopts::value<std::string>())
            ("o,output", "Output shader directory", cxxopts::value<std::string>())
            ("resource", "Config file used for default resource limit in glslang (Not implemented yet)")
            ("remap", "Config file used for attribute remapping in HLSL shader (Not implemented yet)")
            ("fragment", "Compile input as a fragment shader")
            ("vertex", "Compile input as a vertex shader")
            ("geometry", "Compile input as a geometry shader")
            ("control", "Compile input as a tesselation control shader")
            ("eval", "Compile input as a tesselation evaluation shader")
            ("compute", "Compile input as a compute shader")
            ("optimize", "Enable glslang optimizer")
            ("glsl", "Only output opengl shader")
            ("spirv", "Only output vulkan shader")
            ("hlsl", "Only output directx shader")
            ("msl", "Only output metal shader")
            ("all", "Output all shader format (shortcut)")
            ("aggregate", "Output all shader format in one file")
            ("info", "Write reflection shader info in a .json")
            ("sysinclude","Add system include directory (multiple separated by coma)", cxxopts::value<std::vector<std::string>>())
            ("locinclude","Add local include directory (multiple separated by coma)", cxxopts::value<std::vector<std::string>>());

    cxxopts::ParseResult result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (!result.count("input")) {
        std::cout << "Missing shader file.\n\n" << options.help() << std::endl;
        return -1;
    }

    if (!result.count("output")) {
        std::cout << "Missing output.\n\n" << options.help() << std::endl;
        return -1;
    }

    std::filesystem::path shaderInputPath{ result["input"].as<std::string>() };
    std::filesystem::path shaderOutputPath{ result["output"].as<std::string>() };

    if (!std::filesystem::exists(shaderInputPath)) {
        std::cout << "Provided file doesn't exists." << std::endl;
        return -1;
    }

    if (!std::filesystem::is_directory(shaderOutputPath)) {
        std::cout << "Output must be a directory, not a folder." << std::endl;
        return -1;
    }

    std::filesystem::path fileName{ shaderInputPath.filename() };

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

    ShaderCC::ShaderType shaderType{ ShaderCC::ShaderType::None };

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

    if (shaderType == ShaderCC::ShaderType::None) {
        std::cout << "Shader type is missing." << options.help() << std::endl;
        return 0;
    }

    ShaderCC::ShaderIncluder includer{};

    if (result.count("sysinclude")) {
        std::vector<std::string> systemIncludeDir = result["sysinclude"].as<std::vector<std::string>>();
        for (const auto& dir : systemIncludeDir)
            includer.AddSystemIncludeDirectory(dir);
    }

    if (result.count("locinclude")) {
        std::vector<std::string> localIncludeDir = result["locinclude"].as<std::vector<std::string>>();
        for (const auto& dir : localIncludeDir)
            includer.AddLocalIncludeDirectory(dir);
    }

    ShaderCC::Shader shader{ shaderType };

    if (result.count("optimize"))
        shader.SetOptimizer();

    if(!shader.Compile(data, includer)) {
        std::cout << "Failed Compilation." << std::endl;
        return -1;
    }

    bool all = result.count("all");

    if (all || result.count("glsl")) {
        const std::vector<char>& glslSource{ shader.GetGlslSource() };
        std::filesystem::path glslFilePath{ shaderOutputPath / fileName.replace_extension(".glsl") };

        std::ofstream outputFile{ glslFilePath, std::ios_base::binary };
        outputFile.write(glslSource.data(), glslSource.size());
        outputFile.close();

    }
    if (all || result.count("spirv")) {
        const std::vector<char>& spirvSource{ shader.GetSpirVSource() };
        std::filesystem::path spirvFilePath{ shaderOutputPath / fileName.replace_extension(".spirv") };

        std::ofstream outputFile{ spirvFilePath, std::ios_base::binary };
        outputFile.write(spirvSource.data(), spirvSource.size());
        outputFile.close();
    }
    if (all || result.count("hlsl")) {
        const std::vector<char>& hlslSource{ shader.GetHlslSource() };
        std::filesystem::path hlslFilePath{ shaderOutputPath / fileName.replace_extension(".hlsl") };

        std::ofstream outputFile{ hlslFilePath, std::ios_base::binary };
        outputFile.write(hlslSource.data(), hlslSource.size());
        outputFile.close();
    }
    if (all || result.count("msl")) {
        const std::vector<char>& mslSource{ shader.GetMslSource() };
        std::filesystem::path mslFilePath{ shaderOutputPath / fileName.replace_extension(".msl") };

        std::ofstream outputFile{ mslFilePath, std::ios_base::binary };
        outputFile.write(mslSource.data(), mslSource.size());
        outputFile.close();
    }

    if (result.count("aggregate")) {
        std::filesystem::path aggregateFilePath{ shaderOutputPath / fileName.replace_extension(".shader") };
        std::ofstream outputFile{ aggregateFilePath, std::ios_base::binary };
        ShaderCC::WriteAggregateShader(outputFile, shader);
        outputFile.close();
    }

    if (result.count("info")) {
        std::filesystem::path aggregateFilePath{ shaderOutputPath / fileName.replace_extension(".info.json") };
        std::ofstream outputFile{ aggregateFilePath, std::ios_base::binary };

        outputFile << ShaderCC::SerializeShaderInfo(shader).dump(4);

        outputFile.close();
    }
}
