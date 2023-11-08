#ifndef SHADERCROSSCOMPILER_SHADERCROSSCOMPILER_H
#define SHADERCROSSCOMPILER_SHADERCROSSCOMPILER_H

#include <stddef.h>
#include <vector>

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

    class Shader {
    public:
        Shader() = delete;
        explicit Shader(ShaderType type);

        bool Compile(const std::vector<char>& glsl);

        const std::vector<char>& GetGlslSource();
        const std::vector<char>& GetSpirVSource();
        const std::vector<char>& GetMslSource();
        const std::vector<char>& GetHlslSource();

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
    };

    //void CompileProgram(char* data, size_t size);
    //void CompileShader(char* data, size_t size, ShaderType type);
}

#endif //SHADERCROSSCOMPILER_SHADERCROSSCOMPILER_H