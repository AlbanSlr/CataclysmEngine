#include "CataclysmRenderer.hpp"

// std lib headers
#include <array>

namespace Cataclysm
{
    CataclysmRenderer::CataclysmRenderer(CataclysmWindow &window, CataclysmDevice &device) : cataclysmWindow{window}, cataclysmDevice{device}
    {
        recreateSwapChain();
        createCommandBuffers();
    }

    CataclysmRenderer::~CataclysmRenderer()
    {
        freeCommandBuffers();
    }

    void CataclysmRenderer::createCommandBuffers()
    {
        commandBuffers.resize(CataclysmSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = cataclysmDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(cataclysmDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers.");
        }
    }

    void CataclysmRenderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers(cataclysmDevice.device(), cataclysmDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void CataclysmRenderer::recreateSwapChain()
    {
        auto extent = cataclysmWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = cataclysmWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(cataclysmDevice.device());

        if (cataclysmSwapChain == nullptr)
        {
            cataclysmSwapChain = std::make_unique<CataclysmSwapChain>(cataclysmDevice, extent);
        }
        else
        {
            std::shared_ptr<CataclysmSwapChain> oldSwapChain = std::move(cataclysmSwapChain);
            cataclysmSwapChain = std::make_unique<CataclysmSwapChain>(cataclysmDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapChainFormats(*cataclysmSwapChain))
            {
                throw std::runtime_error("Swap chain image (or depth) format has changed.");
            }
        }
    }

    VkCommandBuffer CataclysmRenderer::beginFrame()
    {
        assert(!isFrameStarted && "Cannot call beginFrame while frame is in progress.");

        auto result = cataclysmSwapChain->acquireNextImage(&currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire next image.");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer.");
        }

        return commandBuffer;
    }

    void CataclysmRenderer::endFrame()
    {
        assert(isFrameStarted && "Cannot call endFrame while frame is not in progress.");

        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer.");
        }

        auto result = cataclysmSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || cataclysmWindow.wasWindowResized())
        {
            cataclysmWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image.");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % CataclysmSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void CataclysmRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Cannot begin render pass when frame is not in progress.");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can only begin render pass for command buffer that is being recorded.");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = cataclysmSwapChain->getRenderPass();
        renderPassInfo.framebuffer = cataclysmSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = cataclysmSwapChain->getSwapChainExtent();
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(cataclysmSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(cataclysmSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, cataclysmSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void CataclysmRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Cannot end render pass when frame is not in progress.");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can only end render pass for command buffer that is being recorded.");

        vkCmdEndRenderPass(commandBuffer);
    }
} // namespace Cataclysm