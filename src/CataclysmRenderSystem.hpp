#pragma once

#include "CataclysmDevice.hpp"
#include "CataclysmPipeline.hpp"
#include "CataclysmObject.hpp"
#include "CataclysmCamera.hpp"
#include "CataclysmFrameInfo.hpp"

// std lib headers
#include <memory>
#include <vector>

namespace Cataclysm
{
    class CataclysmRenderSystem
    {
    public:
        CataclysmRenderSystem(CataclysmDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~CataclysmRenderSystem();

        CataclysmRenderSystem(const CataclysmRenderSystem &) = delete;
        CataclysmRenderSystem &operator=(const CataclysmRenderSystem &) = delete;

        void renderGameObjects(FrameInfo &frameInfo, std::vector<CataclysmObject> &gameObjects);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        CataclysmDevice &cataclysmDevice;

        VkPipelineLayout pipelineLayout;
        std::unique_ptr<CataclysmPipeline> cataclysmPipeline;
    };

} // namespace Cataclysm
