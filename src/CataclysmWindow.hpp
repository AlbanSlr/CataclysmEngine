#pragma once

#define VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

// std lib headers
#include <string>

namespace Cataclysm
{
    class CataclysmWindow
    {
    public:
        CataclysmWindow(int width, int height, std::string name);
        ~CataclysmWindow();

        CataclysmWindow(const CataclysmWindow &) = delete;            // disable copy constructor
        CataclysmWindow &operator=(const CataclysmWindow &) = delete; // disable copy assignment

        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
        bool wasWindowResized() { return framebufferResized; }
        void resetWindowResizedFlag() { framebufferResized = false; }
        SDL_Window *getSDLWindow() const { return window; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        bool framebufferResized = false;

    private:
        /* Window & properties */

        SDL_Window *window;

        int width;
        int height;

        const std::string WINDOW_NAME = "Cataclysm";

        void initWindow();

        static void framebufferResizeCallback(SDL_Window *window, int width, int height);
    };
} // namespace Cataclysm