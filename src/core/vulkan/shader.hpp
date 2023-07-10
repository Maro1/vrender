#pragma once

#include "device.hpp"

#include <glslang/Public/ShaderLang.h>

#include <string>
#include <vector>

namespace vrender
{

class Shader
{
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    inline VkShaderModule vertexModule() const { return m_VertModule; }
    inline VkShaderModule fragmentModule() const { return m_FragModule; }

    static std::string readFile(const std::string& path);
private:
    VkShaderModule createShaderModule(const std::string& path);

    static TBuiltInResource shaderResources();

    VkShaderModule m_VertModule;
    VkShaderModule m_FragModule;
};

}; // namespace vrender
