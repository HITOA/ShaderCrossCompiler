#ifndef SHADERCROSSCOMPILER_SHADERINFO_H
#define SHADERCROSSCOMPILER_SHADERINFO_H

#include <json.h>
#include <shadercrosscompiler.h>

namespace ShaderCC {

    inline nlohmann::json SerializeReflectionData(const ReflectionData& reflectionData) {
        nlohmann::json data{};

        for (auto attribute : reflectionData.attributes) {
            nlohmann::json curr{};
            curr["name"] = attribute.name;
            curr["location"] = attribute.location;
            curr["type"]["baseType"] = attribute.type.baseType;
            curr["type"]["row"] = attribute.type.row;
            curr["type"]["col"] = attribute.type.col;
            data["attributes"] += curr;
        }

        return data;
    }

    inline nlohmann::json SerializeShaderInfo(Shader& shader) {
        nlohmann::json shaderInfo{};

        shaderInfo["glsl"] = SerializeReflectionData(shader.GetGlslReflectionData());
        shaderInfo["spirv"] = SerializeReflectionData(shader.GetSpirvReflectionData());
        shaderInfo["hlsl"] = SerializeReflectionData(shader.GetHlslReflectionData());
        shaderInfo["msl"] = SerializeReflectionData(shader.GetMslReflectionData());

        return shaderInfo;
    }

}

#endif //SHADERCROSSCOMPILER_SHADERINFO_H
