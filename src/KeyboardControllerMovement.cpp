#include "KeyboardControllerMovement.hpp"

namespace Cataclysm
{
    void KeyboardControllerMovement::moveInPlaneXZ(SDL_Window *window, CataclysmObject &object, float deltaTime)
    {
        glm::vec3 rotate{0};

        // Get mouse button state
        Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);

        if (mouseState & SDL_BUTTON_LMASK)
        {
            if (!keys.mouseControl)
            {
                keys.mouseControl = true;
                int width, height;
                SDL_GetWindowSize(window, &width, &height);
                SDL_WarpMouseInWindow(window, width / 2.0f, height / 2.0f);
                SDL_SetWindowRelativeMouseMode(window, true);
            }
        }

        // Get keyboard state
        const bool *keyboardState = SDL_GetKeyboardState(nullptr);

        if (keyboardState[keys.escape])
        {
            if (keys.mouseControl)
            {
                keys.mouseControl = false;
                SDL_SetWindowRelativeMouseMode(window, false);
            }
        }

        if (keys.mouseControl)
        {
            int width, height;
            SDL_GetWindowSize(window, &width, &height);

            float mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            float rotX = -lookSpeed * (mouseY - (height / 2.0f)) / (float)height;
            float rotY = lookSpeed * (mouseX - (width / 2.0f)) / (float)width;

            constexpr float maxRotX = glm::radians(89.0f);
            rotX = glm::clamp(rotX, -maxRotX, maxRotX);

            rotate.x += rotX;
            rotate.y += rotY;

            SDL_WarpMouseInWindow(window, width / 2.0f, height / 2.0f);
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

        if (keyboardState[keys.moveForward])
        {
            moveDir += forwardDir;
        }
        if (keyboardState[keys.moveBackward])
        {
            moveDir -= forwardDir;
        }
        if (keyboardState[keys.moveRight])
        {
            moveDir += rightDir;
        }
        if (keyboardState[keys.moveLeft])
        {
            moveDir -= rightDir;
        }
        if (keyboardState[keys.moveUp])
        {
            moveDir += upDir;
        }
        if (keyboardState[keys.moveDown])
        {
            moveDir -= upDir;
        }
        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        {
            object.transform.translation += movementSpeed * deltaTime * glm::normalize(moveDir);
        }
    }

} // namespace Cataclysm