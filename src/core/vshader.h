#pragma once

#include "vdevice.h"

#include <vector>
#include <string>

class VShader 
{
public:

    VShader(const VDevice& device, const std::string& vertexPath, const std::string& fragmentPath);
    ~VShader();

    VShader(const VShader&) = delete;
    VShader& operator=(const VShader&) = delete;

    inline const VkShaderModule& vertexModule() const { return m_VertModule; }
    inline const VkShaderModule& fragmentModule() const { return m_FragModule; }

private:

    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    std::vector<char> m_FragmentData;
    std::vector<char> m_VertexData;

    VkShaderModule m_VertModule;
    VkShaderModule m_FragModule;
    const VDevice& m_Device;
};
