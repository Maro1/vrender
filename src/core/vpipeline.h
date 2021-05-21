#pragma once

#include "core/vdescriptor_set.h"
#include "core/vshader.h"
#include "core/vswap_chain.h"

class VPipeline
{
public:
    VPipeline(VDevice* device, VSwapChain* swapChain);
    ~VPipeline();

    VPipeline(const VPipeline&) = delete;
    VPipeline& operator=(const VPipeline&) = delete;

    void bind(const VkCommandBuffer& commandBuffer);

    inline VkPipelineLayout layout() const
    {
        return m_Layout;
    }

private:
    VShader m_Shader;
    VDevice* m_Device;

    VSwapChain* m_SwapChain;
    VDescriptorSetAllocator* m_DescriptorSetAllocator;

    VkPipeline m_Pipeline;
    VkPipelineLayout m_Layout;
    VkDescriptorSetLayout m_DescriptorSetLayout;

    std::vector<VkDynamicState> m_DynamicStatesEnabled = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    bool createGraphicsPipeline();
};

