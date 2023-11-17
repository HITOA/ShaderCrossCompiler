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
    shader.setAutoMapBindings(true);
    shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

    /*shader.setShiftBinding(glslang::EResUbo, (stage == EShLanguage::EShLangFragment ? 1 : 0));
    shader.setShiftBinding(glslang::EResTexture, 2);
    shader.setShiftBinding(glslang::EResSampler, 2 + 16);
    shader.setShiftBinding(glslang::EResSsbo, 2);
    shader.setShiftBinding(glslang::EResImage, 2);*/

    shader.setStrings(&shSource, 1);

    if (!shader.parse(GetDefaultResources(), 100, false, EShMessages::EShMsgDefault)) {
        std::cout << "Glslang failed to parse shader\n" << shader.getInfoLog() << "\n" << shader.getInfoDebugLog() << std::endl;
        return false;
    }

    glslang::TProgram program{};
    program.addShader(&shader);

    if (!program.link(EShMessages::EShMsgDefault) || !program.mapIO()) {
        std::cout << "Glslang failed to link shader\n" << program.getInfoLog() << "\n" << program.getInfoDebugLog() << std::endl;
        return false;
    }

    program.buildReflection();

    {
        int count = program.getNumPipeInputs();
        for (int i = 0; i < count; ++i) {
            auto& input = program.getPipeInput(i);

            AttributeInfo attributeInfo{};
            attributeInfo.name = input.name;
            attributeInfo.type = input.glDefineType;
            attributeInfo.location = input.getType()->getQualifier().layoutLocation;

            spirvReflectionData.attributes.push_back(attributeInfo);
        }
    }

    {
        int count = program.getNumLiveUniformVariables();
        for (int i = 0; i < count; ++i) {
            auto& uniform = program.getUniform(i);

            UniformInfo uniformInfo{};
            uniformInfo.name = uniform.name;
            uniformInfo.binding = uniform.getBinding();
            uniformInfo.type = uniform.glDefineType;
            uniformInfo.offset = uniform.offset;
            uniformInfo.arraySize = uniform.size;
            uniformInfo.index = uniform.index;
            uniformInfo.texFormat = uniform.getType()->getSampler().type;
            uniformInfo.texDim = uniform.getType()->getSampler().dim;

            spirvReflectionData.uniforms.push_back(uniformInfo);
        }
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

const ShaderCC::ReflectionData& ShaderCC::Shader::GetSpirvReflectionData() {
    return spirvReflectionData;
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