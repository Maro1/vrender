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
    Pipeline();
    ~Pipeline();

    void bind(const VkCommandBuffer& commandBuffer);

    inline VkPipelineLayout layout() const { return m_Layout; }
    inline VkDescriptorSetLayout descriptorSetLayout() const { return m_DescriptorSetLayout; }

private:
    Shader m_Shader;

    VkDescriptorSetLayout m_DescriptorSetLayout;

    VkPipeline m_Pipeline;
    VkPipelineLayout m_Layout;

    std::vector<VkDynamicState> m_DynamicStatesEnabled = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    // TODO: Get from shader instead
    static constexpr uint32_t NUM_DESCRIPTORS = 2;

    bool createGraphicsPipeline();
    bool createGraphicsDescriptorLayout();
};
}; // namespace vrender
