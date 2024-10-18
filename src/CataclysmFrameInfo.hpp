#pragma once

#include "CataclysmCamera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace Cataclysm
{
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        CataclysmCamera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
} // namespace Cataclysm