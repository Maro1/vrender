#pragma once

#include "buffer.hpp"
#include <unordered_map>

namespace vrender
{

struct Uniform
{
    VkDeviceSize size;
    VkDeviceSize offset;
};

class UniformHandler
{
public:
    UniformHandler(VkDeviceSize size);

    template <typename T> void update(const std::string& name, const T& object)
    {
        Uniform uniform = m_Uniforms.find(name)->second;
        m_UniformBuffer.copyData(&object, uniform.size, uniform.offset);
    }

    Buffer* buffer() { return &m_UniformBuffer; }

private:
    Buffer m_UniformBuffer;
    VkDeviceSize m_Size;

    void* m_Data;
    std::unordered_map<std::string, Uniform> m_Uniforms;
};
} // namespace vrender