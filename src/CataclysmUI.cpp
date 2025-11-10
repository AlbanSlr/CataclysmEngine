#include "CataclysmUI.hpp"

// libs
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

namespace Cataclysm
{
    CataclysmUI::CataclysmUI(SDL_Window *window, ImGui_ImplVulkan_InitInfo &initInfo)
        : device{initInfo.Device}
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // Note: Multi-viewport and docking features may not be available in all ImGui builds
        // Uncomment the following lines if your ImGui build supports these features:
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Initialize SDL3 backend
        ImGui_ImplSDL3_InitForVulkan(window);

        // Initialize Vulkan backend
        ImGui_ImplVulkan_Init(&initInfo);
    }

    CataclysmUI::~CataclysmUI()
    {
        // Wait for device to finish all operations before cleanup
        vkDeviceWaitIdle(device);

        // Cleanup ImGui
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

} // namespace Cataclysm