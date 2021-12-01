#include "uniform.hpp"

namespace vrender
{
UniformHandler::UniformHandler(VkDeviceSize size)
    : m_UniformBuffer({size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT})
{
}

} // namespace vrender