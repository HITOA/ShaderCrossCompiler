#ifndef SHADERCROSSCOMPILER_SHADERCROSSCOMPILER_H
#define SHADERCROSSCOMPILER_SHADERCROSSCOMPILER_H

#include <stddef.h>
#include <vector>
#include <string>

namespace ShaderCC {
    enum class ShaderType {
        None,
        Vertex,
        Fragment,
        Geometry,
        TesselationEvaluation,
        TesselationControl,
        Compute,
    };

    struct Type {
        int baseType{};
        int row{};
        int col{};
    };

    struct UniformInfo {
        std::string name{};
        int binding{};
        Type type{};
        int offset{};
        int arraySize{};
        int index{};
        int texFormat{};
        int texDim{};
    };

    struct UniformBlockInfo {
        std::string name{};
        int binding{};
    };

    struct AttributeInfo {
        std::string name{};
        int location{};
        Type type{};
    };

    struct ReflectionData {
        std::vector<UniformInfo> uniforms{};
        std::vector<UniformBlockInfo> ubos{};
        std::vector<AttributeInfo> attributes{};
    };

    class Shader {
    public:
        Shader() = delete;
        explicit Shader(ShaderType type);

        bool Compile(const std::vector<char>& glsl);

        const std::vector<char>& GetGlslSource();
        const std::vector<char>& GetSpirVSource();
        const std::vector<char>& GetMslSource();
        const std::vector<char>& GetHlslSource();

        const ReflectionData& GetGlslReflectionData();
        const ReflectionData& GetSpirvReflectionData();
        const ReflectionData& GetMslReflectionData();
        const ReflectionData& GetHlslReflectionData();

        void SetOptimizer();
    private:
        bool CompileGLSL(std::vector<unsigned int>& spirv);
        bool CompileHLSL(std::vector<unsigned int>& spirv);
        bool CompileMSL(std::vector<unsigned int>& spirv);

    private:
        ShaderType type{};
        bool optimize{ false };
        unsigned int glslVersion{ 100 };

        std::vector<char> glslSource{};
        std::vector<char> spirvSource{};
        std::vector<char> mslSource{};
        std::vector<char> hlslSource{};

        ReflectionData glslReflectionData{};
        ReflectionData spirvReflectionData{};
        ReflectionData mslReflectionData{};
        ReflectionData hlslReflectionData{};
    };
}

#endif //SHADERCROSSCOMPILER_SHADERCROSSCOMPILER_H