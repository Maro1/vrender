#pragma once

#include "core/descriptor_set.h"
#include "core/shader.h"
#include "core/swap_chain.h"

namespace vrender
{
class Pipeline
{
public:
    Pipeline(Device* device, SwapChain* swapChain, DescriptorSetAllocator* descriptorSetAllocator);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void bind(const VkCommandBuffer& commandBuffer);

    inline VkPipelineLayout layout() const { return m_Layout; }

private:
    Shader m_Shader;
    Device* m_Device;

    SwapChain* m_SwapChain;
    DescriptorSetAllocator* m_DescriptorSetAllocator;

    VkPipeline m_Pipeline;
    VkPipelineLayout m_Layout;
    VkDescriptorSetLayout m_DescriptorSetLayout;

    std::vector<VkDynamicState> m_DynamicStatesEnabled = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    bool createGraphicsPipeline();
};
}; // namespace vrender
