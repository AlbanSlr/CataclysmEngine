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
            SDL_Scancode moveLeft = SDL_SCANCODE_A;
            SDL_Scancode moveRight = SDL_SCANCODE_D;
            SDL_Scancode moveForward = SDL_SCANCODE_W;
            SDL_Scancode moveBackward = SDL_SCANCODE_S;
            SDL_Scancode moveUp = SDL_SCANCODE_SPACE;
            SDL_Scancode moveDown = SDL_SCANCODE_LSHIFT;
            SDL_Scancode escape = SDL_SCANCODE_ESCAPE;
            Uint8 leftClick = SDL_BUTTON_LEFT;
            bool mouseControl = false;
        };

        void moveInPlaneXZ(SDL_Window *window, CataclysmObject &object, float deltaTime);

        keyMappings keys{};
        float movementSpeed{3.0f};
        float lookSpeed{1.0f};
    };
} // namespace Cataclysm