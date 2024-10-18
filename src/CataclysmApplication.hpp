#pragma once

#include "CataclysmWindow.hpp"
#include "CataclysmDevice.hpp"
#include "CataclysmObject.hpp"
#include "CataclysmRenderer.hpp"
#include "CataclysmDescriptors.hpp"

// std lib headers
#include <memory>
#include <vector>

namespace Cataclysm
{
    class CataclysmApplication
    {
    public:
        static constexpr int WIDTH = 1188;
        static constexpr int HEIGHT = 648;

        void run();

        CataclysmApplication();
        ~CataclysmApplication();

        CataclysmApplication(const CataclysmApplication &) = delete;
        CataclysmApplication &operator=(const CataclysmApplication &) = delete;

    private:
        void loadGameObjects();

        CataclysmWindow cataclysmWindow{WIDTH, HEIGHT, "Cataclysm Preview Window"};
        CataclysmDevice cataclysmDevice{cataclysmWindow};
        CataclysmRenderer cataclysmRenderer{cataclysmWindow, cataclysmDevice};

        // order of declaration matters
        std::unique_ptr<CataclysmDescriptorPool> globalPool{};
        std::vector<CataclysmObject> gameObjects;
    };

} // namespace Cataclysm
