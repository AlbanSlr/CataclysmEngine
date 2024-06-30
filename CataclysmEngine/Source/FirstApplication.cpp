#include "FirstApplication.hpp"
#include "CataclysmRenderSystem.hpp"
#include "CataclysmCamera.hpp"
#include "KeyboardControllerMovement.hpp"

// std lib headers
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>

namespace Cataclysm
{
    FirstApplication::FirstApplication()
    {
        loadGameObjects();
    }

    FirstApplication::~FirstApplication()
    {
    }

    void FirstApplication::run()
    {
        CataclysmRenderSystem renderSystem{cataclysmDevice, cataclysmRenderer.getSwapChainRenderPass()};
        CataclysmCamera camera{};

        auto viewerObject = CObject::createCataclysmObject();
        KeyboardControllerMovement cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!cataclysmWindow.shouldClose())
        {
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(cataclysmWindow.getGLFWWindow(), viewerObject, frameTime);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = cataclysmRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(45.0f), aspect, 0.1f, 10.0f);

            if (auto commandBuffer = cataclysmRenderer.beginFrame())
            {
                cataclysmRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                cataclysmRenderer.endSwapChainRenderPass(commandBuffer);
                cataclysmRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(cataclysmDevice.device());
    }

    void FirstApplication::loadGameObjects()
    {
        std::shared_ptr<CataclysmModel> cubeModel = CataclysmModel::createModelFromFile(cataclysmDevice, "./Documents/CataclysmEngine/Models/smooth_vase.obj");
        CObject gameObj = CObject::createCataclysmObject();
        gameObj.model = cubeModel;
        gameObj.transform.translation = {0.0f, 0.0f, 2.0f};
        gameObj.transform.scale = glm::vec3(.5f);
        gameObjects.push_back(std::move(gameObj));
    }
} // namespace Cataclysm