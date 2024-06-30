#include "CataclysmRenderSystem.hpp"

// std lib headers
#include <stdexcept>
#include <array>

namespace Cataclysm
{
    struct SimplePushConstantData
    {
        glm::mat4 transform{1.0f};
        alignas(16) glm::vec3 color;
    };

    CataclysmRenderSystem::CataclysmRenderSystem(CataclysmDevice &device, VkRenderPass renderPass) : cataclysmDevice{device}
    {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    CataclysmRenderSystem::~CataclysmRenderSystem()
    {
        vkDeviceWaitIdle(cataclysmDevice.device());
        vkDestroyPipelineLayout(cataclysmDevice.device(), pipelineLayout, nullptr);
    }

    void CataclysmRenderSystem::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(cataclysmDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout.");
        }
    }

    void CataclysmRenderSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

        PipelineConfigInfo pipelineConfig{};
        CataclysmPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        cataclysmPipeline = std::make_unique<CataclysmPipeline>(cataclysmDevice, "./Documents/CataclysmEngine/CataclysmEngine/Source/Shaders/simple_shader.vert.spv", "./Documents/CataclysmEngine/CataclysmEngine/Source/Shaders/simple_shader.frag.spv", pipelineConfig);
    }

    void CataclysmRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<CObject> &gameObjects, const CataclysmCamera &camera)
    {
        cataclysmPipeline->bind(commandBuffer);

        auto projectionView = camera.getProjection() * camera.getView();

        for (auto &obj : gameObjects)
        {
            SimplePushConstantData push{};
            push.color = obj.color;
            push.transform = projectionView * obj.transform.mat4();
            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }

} // namespace Cataclysm