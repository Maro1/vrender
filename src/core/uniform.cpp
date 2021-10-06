#include "uniform.h"

namespace vrender
{
UniformHandler::UniformHandler(VkDeviceSize size)
    : m_UniformBuffer({size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT})
{
}

template <typename T> void UniformHandler::update(const std::string& name, const T& object)
{
    Uniform uniform = m_Uniforms.find(name);
    m_UniformBuffer.copyData(&object, uniform.size, uniform.offset);
}

} // namespace vrender