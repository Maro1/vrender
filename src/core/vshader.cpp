#include "vshader.h"
#include "utils/vlog.h"

#include <cstddef>
#include <fstream>
#include <type_traits>

VShader::VShader(const VDevice& device, const std::string& vertexPath, const std::string& fragmentPath) : m_Device(device)
{
    m_VertexData = VShader::readFile(vertexPath);
    m_FragmentData = VShader::readFile(fragmentPath);
    m_VertModule = createShaderModule(m_VertexData);
    m_FragModule = createShaderModule(m_FragmentData);
}

VShader::~VShader()
{
    vkDestroyShaderModule(m_Device.device(), m_VertModule, nullptr);
    vkDestroyShaderModule(m_Device.device(), m_FragModule, nullptr);
}

std::vector<char> VShader::readFile(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        V_LOG_ERROR("Could not open shader file with path: {}", filename);
        return std::vector<char>();
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}


VkShaderModule VShader::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_Device.device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }
    return shaderModule;
}
