#pragma once

#include "core/vdevice.h"
#include "core/vswap_chain.h"
#include "core/vpipeline.h"

#include <vector>

class VRenderer
{
public:

   VRenderer(const VDevice& device, VSwapChain& swapChain, VWindow& window);
   ~VRenderer();

   VRenderer(const VRenderer&) = delete;
   VRenderer& operator=(const VRenderer&) = delete;

   VkCommandBuffer beginFrame();
   VkResult endFrame();
   void beginRenderPass();
   void endRenderPass();
   void draw(const VkCommandBuffer& commandBuffer); // TODO: MOVE TO SCENE GRAPH

private:

   void init();

   VkResult createCommandBuffers();
    
   const VDevice& m_Device;
   VSwapChain& m_SwapChain;
   VWindow& m_Window;

   std::vector<VkFence> m_InFlightFences;
   std::vector<VkCommandBuffer> m_CommandBuffers;
   uint32_t m_CurrentFrame = 0;
   uint32_t m_CurrentImage;

   std::vector<VkSemaphore> m_RenderFinishedSemaphores;
};
