#include "CataclysmRenderSystem.hpp"

// std lib headers
#include <stdexcept>
#include <array>

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif

std::string SHADER_PATH = SHADER_DIR;

namespace Cataclysm
{
    struct SimplePushConstantData
    {
        glm::mat4 modelMatrix{1.0f};
        glm::mat4 normalMatrix{1.0f};
    };

    CataclysmRenderSystem::CataclysmRenderSystem(CataclysmDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : cataclysmDevice{device}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    CataclysmRenderSystem::~CataclysmRenderSystem()
    {
        vkDeviceWaitIdle(cataclysmDevice.device());
        vkDestroyPipelineLayout(cataclysmDevice.device(), pipelineLayout, nullptr);
    }

    void CataclysmRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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
        cataclysmPipeline = std::make_unique<CataclysmPipeline>(cataclysmDevice, SHADER_PATH + "simple_shader.vert.spv", SHADER_PATH + "simple_shader.frag.spv", pipelineConfig);
    }

    void CataclysmRenderSystem::renderGameObjects(FrameInfo &frameInfo, std::vector<CataclysmObject> &gameObjects)
    {
        cataclysmPipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

        for (auto &obj : gameObjects)
        {
            SimplePushConstantData push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();
            vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }

} // namespace Cataclysm