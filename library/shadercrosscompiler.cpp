#include "shadercrosscompiler.h"
#include <iostream>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <spirv_glsl.hpp>
#include <spirv_hlsl.hpp>
#include <spirv_msl.hpp>

EShLanguage GetGlslangStageFromShaderType(ShaderCC::ShaderType type) {
    switch (type) {
        case ShaderCC::ShaderType::Fragment:
            return EShLanguage::EShLangFragment;
        case ShaderCC::ShaderType::Vertex:
            return EShLanguage::EShLangVertex;
        case ShaderCC::ShaderType::Geometry:
            return EShLanguage::EShLangGeometry;
        case ShaderCC::ShaderType::TesselationControl:
            return EShLanguage::EShLangTessControl;
        case ShaderCC::ShaderType::TesselationEvaluation:
            return EShLanguage::EShLangTessEvaluation;
        case ShaderCC::ShaderType::Compute:
            return EShLanguage::EShLangCompute;
        default:
            return EShLanguage::EShLangVertex;
    }
}

ShaderCC::Shader::Shader(ShaderCC::ShaderType type) : type{ type } {}

bool ShaderCC::Shader::Compile(const std::vector<char>& glsl) {
    glslang::InitializeProcess();
    const char* shSource = glsl.data();

    EShLanguage stage = GetGlslangStageFromShaderType(type);

    glslang::TShader shader{ stage };
    shader.setStrings(&shSource, 1);
    shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

    if (!shader.parse(GetDefaultResources(), 100, false, EShMessages::EShMsgDefault)) {
        std::cout << "Glslang failed to parse shader\n" << shader.getInfoLog() << "\n" << shader.getInfoDebugLog() << std::endl;
        return false;
    }

    glslang::TProgram program{};
    program.addShader(&shader);

    if (!program.link(EShMessages::EShMsgDefault)) {
        std::cout << "Glslang failed to link shader\n" << program.getInfoLog() << "\n" << program.getInfoDebugLog() << std::endl;
        return false;
    }

    std::vector<unsigned int> spirv{};
    glslang::SpvOptions spvOptions{};
    spvOptions.disableOptimizer = !optimize;
    glslang::TIntermediate* intermediate = program.getIntermediate(stage);

    glslVersion = intermediate->getVersion();

    glslang::GlslangToSpv(*intermediate, spirv, &spvOptions);

    spirvSource.resize(spirv.size() * sizeof(unsigned int));
    memcpy(spirvSource.data(), spirv.data(), spirvSource.size());

    glslang::FinalizeProcess();

    if(!CompileGLSL(spirv)) {
        std::cout << "Couldn't compile glsl." << std::endl;
        return false;
    }
    if(!CompileHLSL(spirv)) {
        std::cout << "Couldn't compile hlsl." << std::endl;
        return false;
    }
    if(!CompileMSL(spirv)) {
        std::cout << "Couldn't compile msl." << std::endl;
        return false;
    }

    return true;
}

const std::vector<char>& ShaderCC::Shader::GetGlslSource() {
    return glslSource;
}

const std::vector<char>& ShaderCC::Shader::GetSpirVSource() {
    return spirvSource;
}

const std::vector<char>& ShaderCC::Shader::GetHlslSource() {
    return hlslSource;
}

const std::vector<char>& ShaderCC::Shader::GetMslSource() {
    return mslSource;
}

void ShaderCC::Shader::SetOptimizer() {
    optimize = true;
}

bool ShaderCC::Shader::CompileGLSL(std::vector<unsigned int> &spirv) {
    spirv_cross::CompilerGLSL glsl{ spirv };

    spirv_cross::CompilerGLSL::Options options{};
    options.version = glslVersion;

    glsl.set_common_options(options);
    std::string source = glsl.compile();
    glslSource.resize(source.size());
    memcpy(glslSource.data(), source.data(), glslSource.size());
    return true;
}

bool ShaderCC::Shader::CompileHLSL(std::vector<unsigned int> &spirv) {
    spirv_cross::CompilerHLSL hlsl{ spirv };

    spirv_cross::CompilerHLSL::Options options{};
    options.shader_model = glslVersion >= 400 ? 40 : 30;

    hlsl.set_hlsl_options(options);
    std::string source = hlsl.compile();
    hlslSource.resize(source.size());
    memcpy(hlslSource.data(), source.data(), hlslSource.size());
    return true;
}

bool ShaderCC::Shader::CompileMSL(std::vector<unsigned int> &spirv) {
    spirv_cross::CompilerMSL msl{ spirv };
    std::string source = msl.compile();
    mslSource.resize(source.size());
    memcpy(mslSource.data(), source.data(), mslSource.size());
    return true;
}