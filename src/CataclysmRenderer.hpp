#pragma once

#include "CataclysmWindow.hpp"
#include "CataclysmDevice.hpp"
#include "CataclysmSwapChain.hpp"

// std lib headers
#include <cassert>
#include <memory>
#include <vector>

namespace Cataclysm
{
    class CataclysmRenderer
    {
    public:
        CataclysmRenderer(CataclysmWindow &window, CataclysmDevice &device);
        ~CataclysmRenderer();

        CataclysmRenderer(const CataclysmRenderer &) = delete;
        CataclysmRenderer &operator=(const CataclysmRenderer &) = delete;

        VkRenderPass getSwapChainRenderPass() const { return cataclysmSwapChain->getRenderPass(); }
        float getAspectRatio() const { return cataclysmSwapChain->extentAspectRatio(); }
        CataclysmSwapChain *getSwapChain() const { return cataclysmSwapChain.get(); }

        bool isFrameInProgress() const { return isFrameStarted; }
        VkCommandBuffer getCurrentCommandBuffer() const
        {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress.");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress.");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        CataclysmWindow &cataclysmWindow;
        CataclysmDevice &cataclysmDevice;
        std::unique_ptr<CataclysmSwapChain> cataclysmSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };

} // namespace Cataclysm
