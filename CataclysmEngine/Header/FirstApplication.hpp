#pragma once

#include "CataclysmWindow.hpp"
#include "CataclysmDevice.hpp"
#include "CObject.hpp"
#include "CataclysmRenderer.hpp"

// std lib headers
#include <memory>
#include <vector>

namespace Cataclysm
{
    class FirstApplication
    {
    public:
        static constexpr int WIDTH = 1188;
        static constexpr int HEIGHT = 648;

        void run();

        FirstApplication();
        ~FirstApplication();

        FirstApplication(const FirstApplication &) = delete;
        FirstApplication &operator=(const FirstApplication &) = delete;

    private:
        void loadGameObjects();

        CataclysmWindow cataclysmWindow{WIDTH, HEIGHT, "Cataclysm Preview Window"};
        CataclysmDevice cataclysmDevice{cataclysmWindow};
        CataclysmRenderer cataclysmRenderer{cataclysmWindow, cataclysmDevice};

        std::vector<CObject> gameObjects;
    };

} // namespace Cataclysm
