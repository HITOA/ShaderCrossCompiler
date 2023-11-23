#ifndef SHADERCROSSCOMPILER_SHADERCROSSCOMPILER_H
#define SHADERCROSSCOMPILER_SHADERCROSSCOMPILER_H

#include <stddef.h>
#include <vector>
#include <string>
#include <glslang/Public/ShaderLang.h>

#define MAX_INCLUDE_DEPTH 256

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

    struct UniformInfo {
        std::string name{};
        int binding{};
        int type{};
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
        int type{};
    };

    struct ReflectionData {
        std::vector<UniformInfo> uniforms{};
        std::vector<UniformBlockInfo> ubos{};
        std::vector<AttributeInfo> attributes{};
    };

    class ShaderIncluder : public glslang::TShader::Includer {
    public:
        // For the "system" or <>-style includes; search the "system" paths.
        IncludeResult* includeSystem(const char* /*headerName*/, const char* /*includerName*/, size_t /*inclusionDepth*/) final;

        // For the "local"-only aspect of a "" include. Should not search in the
        // "system" paths, because on returning a failure, the parser will
        // call includeSystem() to look in the "system" locations.
        IncludeResult* includeLocal(const char* /*headerName*/, const char* /*includerName*/, size_t /*inclusionDepth*/) final;

        // Signals that the parser will no longer use the contents of the
        // specified IncludeResult.
        void releaseInclude(IncludeResult*) final;

        void AddSystemIncludeDirectory(const std::string& dir);
        void AddLocalIncludeDirectory(const std::string& dir);

    private:
        std::vector<std::string> systemIncludeDir{};
        std::vector<std::string> localIncludeDir{};
    };

    class Shader {
    public:
        Shader() = delete;
        explicit Shader(ShaderType type);

        bool Compile(const std::vector<char>& glsl, ShaderIncluder& includer);
        bool Compile(const std::vector<char>& glsl);

        const std::vector<char>& GetGlslSource();
        const std::vector<char>& GetSpirVSource();
        const std::vector<char>& GetMslSource();
        const std::vector<char>& GetHlslSource();

        const ReflectionData& GetSpirvReflectionData();

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

        ReflectionData spirvReflectionData{};
    };
}

#endif //SHADERCROSSCOMPILER_SHADERCROSSCOMPILER_H