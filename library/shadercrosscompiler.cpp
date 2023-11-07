#include "shadercrosscompiler.h"
#include <iostream>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/SPIRV/GlslangToSpv.h>

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

    glslang::TShader shader{ GetGlslangStageFromShaderType(type) };
    shader.setStrings(&shSource, 1);
    shader.setEnvInput(glslang::EShSourceGlsl, GetGlslangStageFromShaderType(type), glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

    if (!shader.parse(GetDefaultResources(), 100, false, EShMessages::EShMsgDefault)) {
        std::cout << "Glslang failed to parse shader\n" << shader.getInfoLog() << "\n" << shader.getInfoDebugLog() << std::endl;
        return false;
    }

    /*glslang::TProgram program{};
    program.addShader(&shader);

    if (!program.link(EShMessages::EShMsgDefault)) {
        std::cout << "Glslang failed to link shader\n" << program.getInfoLog() << "\n" << program.getInfoDebugLog() << std::endl;
        return;
    }*/

    std::vector<unsigned int> spirv{};
    glslang::SpvOptions spvOptions{};
    spvOptions.disableOptimizer = !optimize;
    glslang::TIntermediate* intermediate = shader.getIntermediate();

    glslang::GlslangToSpv(*intermediate, spirv, &spvOptions);

    spirvSource.resize(spirv.size() * sizeof(unsigned int));
    memcpy(spirvSource.data(), spirv.data(), spirvSource.size());

    glslang::FinalizeProcess();

    return true;
}

const std::vector<char>& ShaderCC::Shader::GetSpirVSource() {
    return spirvSource;
}

void ShaderCC::Shader::SetOptimizer() {
    optimize = true;
}