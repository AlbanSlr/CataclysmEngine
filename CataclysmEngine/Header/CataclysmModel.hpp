#pragma once

#include "CataclysmDevice.hpp"

// std lib headers
#include <memory>
#include <vector>

// glm stuff
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Cataclysm
{
    class CataclysmModel
    {
    public:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec3 normal;
            glm::vec2 texCoord;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string &filepath);
        };

        CataclysmModel(CataclysmDevice &device, const CataclysmModel::Builder &builder);
        ~CataclysmModel();

        CataclysmModel(const CataclysmModel &) = delete;
        CataclysmModel &operator=(const CataclysmModel &) = delete;

        static std::unique_ptr<CataclysmModel> createModelFromFile(CataclysmDevice &device, const std::string &filepath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffer(const std::vector<Vertex> &vertices);
        void createIndexBuffer(const std::vector<uint32_t> &indices);

        CataclysmDevice &cataclysmDevice;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
    };
} // namespace Cataclysm