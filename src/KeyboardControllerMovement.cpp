#include "KeyboardControllerMovement.hpp"

namespace Cataclysm
{
    void KeyboardControllerMovement::moveInPlaneXZ(GLFWwindow *window, CataclysmObject &object, float deltaTime)
    {
        glm::vec3 rotate{0};

        if (glfwGetMouseButton(window, keys.leftClick) == GLFW_PRESS)
        {
            keys.mouseControl = true;
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            glfwSetCursorPos(window, width / 2, height / 2);
            glfwSetInputMode(window, keys.cursor, GLFW_CURSOR_DISABLED);
        }

        if (glfwGetKey(window, keys.escape) == GLFW_PRESS)
        {
            keys.mouseControl = false;
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            glfwSetCursorPos(window, width / 2, height / 2);
            glfwSetInputMode(window, keys.cursor, GLFW_CURSOR_NORMAL);
        }

        if (keys.mouseControl)
        {
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            float rotX = -lookSpeed * (float)(mouseY - (height / 2)) / (float)height;
            float rotY = lookSpeed * (float)(mouseX - (width / 2)) / (float)width;

            rotate.x += rotX;
            rotate.y += rotY;

            glfwSetCursorPos(window, width / 2, height / 2);
        }

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        {
            object.transform.rotation += movementSpeed * deltaTime * glm::normalize(rotate);
        }

        object.transform.rotation.x = glm::clamp(object.transform.rotation.x, -1.5f, 1.5f);
        object.transform.rotation.y = glm::mod(object.transform.rotation.y, glm::two_pi<float>());

        float yaw = object.transform.rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
        const glm::vec3 upDir{0.0f, -1.0f, 0.0f};

        glm::vec3 moveDir{0.0f};

        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
        {
            moveDir += forwardDir;
        }
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
        {
            moveDir -= forwardDir;
        }
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
        {
            moveDir += rightDir;
        }
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
        {
            moveDir -= rightDir;
        }
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
        {
            moveDir += upDir;
        }
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
        {
            moveDir -= upDir;
        }
        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        {
            object.transform.translation += movementSpeed * deltaTime * glm::normalize(moveDir);
        }
    }

} // namespace Cataclysm