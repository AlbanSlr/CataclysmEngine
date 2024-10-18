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
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, WINDOW_NAME.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    CataclysmWindow::~CataclysmWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool CataclysmWindow::shouldClose()
    {
        return glfwWindowShouldClose(window);
    }

    void CataclysmWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void CataclysmWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto cataclysmWindow = reinterpret_cast<CataclysmWindow *>(glfwGetWindowUserPointer(window));
        cataclysmWindow->framebufferResized = true;
        cataclysmWindow->width = width;
        cataclysmWindow->height = height;
    }
} // namespace Cataclysm