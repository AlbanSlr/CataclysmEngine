#include "CataclysmApplication.hpp"
#include "CataclysmRenderSystem.hpp"
#include "CataclysmCamera.hpp"
#include "KeyboardControllerMovement.hpp"
#include "CataclysmBuffer.hpp"

// ImGui
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

// std lib headers
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>

#ifndef MODEL_DIR
#error "MODEL_DIR not defined"
#endif

std::string MODEL_PATH = MODEL_DIR;

namespace Cataclysm
{
    struct GlobalUbo
    {
        glm::mat4 projectionView{1.0f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -1.0f, 1.0f});
    };

    CataclysmApplication::CataclysmApplication()
    {
        globalPool = CataclysmDescriptorPool::Builder(cataclysmDevice).setMaxSets(CataclysmSwapChain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, CataclysmSwapChain::MAX_FRAMES_IN_FLIGHT).build();
        loadGameObjects();

        // creating a descriptor pool for ImGui
        imguiPool = CataclysmDescriptorPool::Builder(cataclysmDevice)
                        .setMaxSets(1000)
                        .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
                        .build();

        // getting the vulkan initialization info for ImGui
        auto queueFamily = cataclysmDevice.findPhysicalQueueFamilies();

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = cataclysmDevice.getInstance();
        init_info.PhysicalDevice = cataclysmDevice.getPhysicalDevice();
        init_info.Device = cataclysmDevice.device();
        init_info.QueueFamily = queueFamily.graphicsFamily;
        init_info.Queue = cataclysmDevice.graphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = imguiPool->getDescriptorPool();
        init_info.RenderPass = cataclysmRenderer.getSwapChainRenderPass();
        init_info.Subpass = 0;
        init_info.MinImageCount = 2;
        init_info.ImageCount = static_cast<uint32_t>(cataclysmRenderer.getSwapChain()->imageCount());
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = [](VkResult err)
        {
            if (err == VK_SUCCESS)
                return;
            std::cerr << "[vulkan] Error: VkResult = " << err << std::endl;
            if (err < 0)
                throw std::runtime_error("Vulkan error in ImGui");
        };

        // creating the UI
        cataclysmUI = std::make_unique<CataclysmUI>(cataclysmWindow.getSDLWindow(), init_info);
    }

    CataclysmApplication::~CataclysmApplication()
    {
        // Destroy ImGui resources before destroying Vulkan device
        cataclysmUI.reset();
        imguiPool.reset();
    }

    void CataclysmApplication::run()
    {
        std::vector<std::unique_ptr<CataclysmBuffer>> uboBuffers(CataclysmSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<CataclysmBuffer>(cataclysmDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, cataclysmDevice.properties.limits.minUniformBufferOffsetAlignment);
            uboBuffers[i]->map();
        }

        auto globalSetLayout = CataclysmDescriptorSetLayout::Builder(cataclysmDevice)
                                   .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                                   .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(CataclysmSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            CataclysmDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        CataclysmRenderSystem renderSystem{cataclysmDevice, cataclysmRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        CataclysmCamera camera{};

        auto viewerObject = CataclysmObject::createCataclysmObject();
        KeyboardControllerMovement cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        bool running = true;
        SDL_Event event;

        while (running)
        {

            // Handle events
            while (SDL_PollEvent(&event))
            {
                // Pass events to ImGui
                ImGui_ImplSDL3_ProcessEvent(&event);

                switch (event.type)
                {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    // Handle window resize - the renderer will recreate swap chain automatically
                    cataclysmWindow.framebufferResized = true;
                    break;
                }
            }

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(cataclysmWindow.getSDLWindow(), viewerObject, frameTime);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = cataclysmRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(45.0f), aspect, 0.1f, 10.0f);

            if (auto commandBuffer = cataclysmRenderer.beginFrame())
            {
                int frameIndex = cataclysmRenderer.getFrameIndex();
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

                // Start ImGui frame
                ImGui_ImplVulkan_NewFrame();
                ImGui_ImplSDL3_NewFrame();
                ImGui::NewFrame();

                // ImGui UI - Example debug window
                ImGui::Begin("Debug Info");
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                            1000.0f / ImGui::GetIO().Framerate,
                            ImGui::GetIO().Framerate);
                ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)",
                            viewerObject.transform.translation.x,
                            viewerObject.transform.translation.y,
                            viewerObject.transform.translation.z);
                ImGui::End();

                // Render ImGui
                ImGui::Render();

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                cataclysmRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(frameInfo, gameObjects);

                // Draw ImGui
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

                cataclysmRenderer.endSwapChainRenderPass(commandBuffer);
                cataclysmRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(cataclysmDevice.device());
    }

    void CataclysmApplication::loadGameObjects()
    {
        std::shared_ptr<CataclysmModel> cubeModel = CataclysmModel::createModelFromFile(cataclysmDevice, MODEL_PATH + "smooth_vase.obj");
        CataclysmObject gameObj = CataclysmObject::createCataclysmObject();
        gameObj.model = cubeModel;
        gameObj.transform.translation = {0.0f, 0.0f, 2.0f};
        gameObj.transform.scale = glm::vec3(.5f);
        gameObjects.push_back(std::move(gameObj));
    }
} // namespace Cataclysm