#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

// std lib headers
#include <string>

namespace Cataclysm
{
    class CataclysmWindow
    {
    private:
        /* Window & properties */

        GLFWwindow *window;

        int width;
        int height;
        bool framebufferResized = false;

        const std::string WINDOW_NAME = "Cataclysm";

        void initWindow();

        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    public:
        CataclysmWindow(int width, int height, std::string name);
        ~CataclysmWindow();

        CataclysmWindow(const CataclysmWindow &) = delete;            // disable copy constructor
        CataclysmWindow &operator=(const CataclysmWindow &) = delete; // disable copy assignment

        bool shouldClose();
        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
        bool wasWindowResized() { return framebufferResized; }
        void resetWindowResizedFlag() { framebufferResized = false; }
        GLFWwindow *getGLFWWindow() const { return window; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
    };
} // namespace Cataclysm