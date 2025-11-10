#pragma once

// libs
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <imgui_impl_vulkan.h>

namespace Cataclysm
{
    class CataclysmUI
    {

    public:
        CataclysmUI(SDL_Window *window, ImGui_ImplVulkan_InitInfo &initInfo);
        ~CataclysmUI();

    private:
        VkDevice device;
    };

} // namespace Cataclysm