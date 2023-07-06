#include "shader.hpp"

#include "core/graphics_context.hpp"

#include "utils/log.hpp"

#include <cstddef>
#include <fstream>
#include <type_traits>

namespace vrender
{

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    m_VertexData = Shader::readFile(vertexPath);
    m_FragmentData = Shader::readFile(fragmentPath);
    m_VertModule = createShaderModule(m_VertexData);
    m_FragModule = createShaderModule(m_FragmentData);
}

Shader::~Shader()
{
    vkDestroyShaderModule(GraphicsContext::get().device()->device(), m_VertModule, nullptr);
    vkDestroyShaderModule(GraphicsContext::get().device()->device(), m_FragModule, nullptr);
}

std::vector<char> Shader::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        V_LOG_ERROR("Could not open shader file with path: {}", filename);
        return std::vector<char>();
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

VkShaderModule Shader::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(GraphicsContext::get().device()->device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        return VK_NULL_HANDLE;
    }
    return shaderModule;
}
}; // namespace vrender
