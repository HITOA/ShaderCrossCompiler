#ifndef SHADERCROSSCOMPILER_SHADERINFO_H
#define SHADERCROSSCOMPILER_SHADERINFO_H

#include <json.h>
#include <shadercrosscompiler.h>

namespace ShaderCC {

    inline nlohmann::json SerializeReflectionData(const ReflectionData& reflectionData) {
        nlohmann::json data{};

        for (const auto& attribute : reflectionData.attributes) {
            nlohmann::json curr{};
            curr["name"] = attribute.name;
            curr["location"] = attribute.location;
            curr["type"]= attribute.type;
            data["attributes"] += curr;
        }

        for (const auto& uniform : reflectionData.uniforms) {
            nlohmann::json curr{};
            curr["name"] = uniform.name;
            curr["binding"] = uniform.binding;
            curr["type"]= uniform.type;
            curr["offset"]= uniform.offset;
            curr["arraySize"]= uniform.arraySize;
            curr["index"]= uniform.index;
            curr["texFormat"]= uniform.texFormat;
            curr["texDim"]= uniform.texDim;
            data["uniforms"] += curr;
        }

        return data;
    }

    inline nlohmann::json SerializeShaderInfo(Shader& shader) {
        nlohmann::json shaderInfo{};

        shaderInfo["spirv"] = SerializeReflectionData(shader.GetSpirvReflectionData());

        return shaderInfo;
    }

}

#endif //SHADERCROSSCOMPILER_SHADERINFO_H
