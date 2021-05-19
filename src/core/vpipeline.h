#pragma once

#include "core/vshader.h"
#include "core/vswap_chain.h"
#include "core/vvertex_buffer.h"

class VPipeline
{
public:
    VPipeline(const VDevice& device, const VSwapChain& swapChain);
    ~VPipeline();
    
    VPipeline(const VPipeline&) = delete;
    VPipeline& operator=(const VPipeline&) = delete;

    void bind(const VkCommandBuffer& commandBuffer);

private:
    VShader m_Shader;
    const VDevice& m_Device;
    const VSwapChain& m_SwapChain;

    VkPipeline m_Pipeline;
    VkPipelineLayout m_Layout;

    std::vector<VkDynamicState> m_DynamicStatesEnabled = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    bool createGraphicsPipeline();
};

