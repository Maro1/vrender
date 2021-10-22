#pragma once

#include "core/vulkan/descriptor_set.hpp"
#include "core/vulkan/shader.hpp"
#include "core/vulkan/swap_chain.hpp"

#include "utils/noncopyable.hpp"

namespace vrender
{
class Pipeline : private NonCopyable
{
public:
    Pipeline(Device* device, SwapChain* swapChain);
    ~Pipeline();

    void bind(const VkCommandBuffer& commandBuffer);

    inline VkPipelineLayout layout() const { return m_Layout; }

private:
    Shader m_Shader;
    Device* m_Device;

    SwapChain* m_SwapChain;
    DescriptorSetAllocator m_DescriptorSetAllocator;
    DescriptorPool m_DescriptorPool;

    VkPipeline m_Pipeline;
    VkPipelineLayout m_Layout;
    VkDescriptorSetLayout m_DescriptorSetLayout;

    std::vector<VkDynamicState> m_DynamicStatesEnabled = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    bool createGraphicsPipeline();
};
}; // namespace vrender
