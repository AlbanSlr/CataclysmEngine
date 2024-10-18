#pragma once

#include "CataclysmModel.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std lib headers
#include <memory>

namespace Cataclysm
{
    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    class CataclysmObject
    {
    public:
        using id_t = unsigned int;

        static CataclysmObject createCataclysmObject()
        {
            static id_t currentId = 0;
            return CataclysmObject{currentId++};
        }

        CataclysmObject(const CataclysmObject &) = delete;
        CataclysmObject &operator=(const CataclysmObject &) = delete;
        CataclysmObject(CataclysmObject &&) = default;
        CataclysmObject &operator=(CataclysmObject &&) = default;

        id_t getId() const { return id; }

        std::shared_ptr<CataclysmModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        CataclysmObject(id_t objId) : id{objId} {}

        id_t id;
    };
} // namespace Cataclysm