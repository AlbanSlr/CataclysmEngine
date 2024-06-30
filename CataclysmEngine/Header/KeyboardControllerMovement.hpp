#pragma once

#include "CObject.hpp"
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
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(GLFWwindow *window, CObject &object, float deltaTime);

        keyMappings keys{};
        float movementSpeed{3.0f};
        float lookSpeed{1.5f};
    };
} // namespace Cataclysm