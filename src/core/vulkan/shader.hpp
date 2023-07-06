#pragma once

#include "device.hpp"

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

private:
    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    std::vector<char> m_FragmentData;
    std::vector<char> m_VertexData;

    VkShaderModule m_VertModule;
    VkShaderModule m_FragModule;
};

}; // namespace vrender
