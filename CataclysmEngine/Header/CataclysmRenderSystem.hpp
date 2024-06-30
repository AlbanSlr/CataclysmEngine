#pragma once

#include "CataclysmDevice.hpp"
#include "CataclysmPipeline.hpp"
#include "CObject.hpp"
#include "CataclysmCamera.hpp"

// std lib headers
#include <memory>
#include <vector>

namespace Cataclysm
{
    class CataclysmRenderSystem
    {
    public:
        CataclysmRenderSystem(CataclysmDevice &device, VkRenderPass renderPass);
        ~CataclysmRenderSystem();

        CataclysmRenderSystem(const CataclysmRenderSystem &) = delete;
        CataclysmRenderSystem &operator=(const CataclysmRenderSystem &) = delete;

        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<CObject> &gameObjects, const CataclysmCamera &camera);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        CataclysmDevice &cataclysmDevice;

        VkPipelineLayout pipelineLayout;
        std::unique_ptr<CataclysmPipeline> cataclysmPipeline;
    };

} // namespace Cataclysm
