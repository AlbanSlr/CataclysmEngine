#include "CataclysmWindow.hpp"

// std lib headers
#include <stdexcept>

namespace Cataclysm
{
    CataclysmWindow::CataclysmWindow(int width, int height, std::string name) : width{width}, height{height}, WINDOW_NAME{name}
    {
        initWindow();
    }

    void CataclysmWindow::initWindow()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            throw std::runtime_error("failed to initialize SDL3!");
        }

        window = SDL_CreateWindow(WINDOW_NAME.c_str(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

        if (!window)
        {
            throw std::runtime_error("failed to create SDL3 window!");
        }

        // Store pointer to this instance using SDL3 properties
        SDL_SetPointerProperty(SDL_GetWindowProperties(window), "CataclysmWindow", this);
    }

    CataclysmWindow::~CataclysmWindow()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void CataclysmWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, surface))
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void CataclysmWindow::framebufferResizeCallback(SDL_Window *window, int width, int height)
    {
        auto cataclysmWindow = reinterpret_cast<CataclysmWindow *>(
            SDL_GetPointerProperty(SDL_GetWindowProperties(window), "CataclysmWindow", nullptr));
        if (cataclysmWindow)
        {
            cataclysmWindow->framebufferResized = true;
            cataclysmWindow->width = width;
            cataclysmWindow->height = height;
        }
    }
} // namespace Cataclysm