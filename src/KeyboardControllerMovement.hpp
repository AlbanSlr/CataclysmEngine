#pragma once

#include "CataclysmObject.hpp"
#include "CataclysmWindow.hpp"

namespace Cataclysm
{
    class KeyboardControllerMovement
    {
    public:
        struct keyMappings
        {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_LEFT_SHIFT;
            int escape = GLFW_KEY_ESCAPE;
            int leftClick = GLFW_MOUSE_BUTTON_LEFT;
            int cursor = GLFW_CURSOR;
            bool mouseControl = false;
        };

        void moveInPlaneXZ(GLFWwindow *window, CataclysmObject &object, float deltaTime);

        keyMappings keys{};
        float movementSpeed{3.0f};
        float lookSpeed{100.0f};
    };
} // namespace Cataclysm