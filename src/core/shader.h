#pragma once

#include "device.h"

#include <string>
#include <vector>

namespace vrender
{

class Shader
{
public:
    Shader(Device* device, const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    inline const VkShaderModule& vertexModule() const { return m_VertModule; }
    inline const VkShaderModule& fragmentModule() const { return m_FragModule; }

private:
    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    std::vector<char> m_FragmentData;
    std::vector<char> m_VertexData;

    VkShaderModule m_VertModule;
    VkShaderModule m_FragModule;
    Device* m_Device;
};

}; // namespace vrender
