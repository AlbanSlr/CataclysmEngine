#pragma once
#include "CataclysmDevice.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace Cataclysm
{

    class CataclysmDescriptorSetLayout
    {
    public:
        class Builder
        {
        public:
            Builder(CataclysmDevice &cataclysmDevice) : cataclysmDevice{cataclysmDevice} {}

            Builder &addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<CataclysmDescriptorSetLayout> build() const;

        private:
            CataclysmDevice &cataclysmDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        CataclysmDescriptorSetLayout(
            CataclysmDevice &cataclysmDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~CataclysmDescriptorSetLayout();
        CataclysmDescriptorSetLayout(const CataclysmDescriptorSetLayout &) = delete;
        CataclysmDescriptorSetLayout &operator=(const CataclysmDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        CataclysmDevice &cataclysmDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class CataclysmDescriptorWriter;
    };

    class CataclysmDescriptorPool
    {
    public:
        class Builder
        {
        public:
            Builder(CataclysmDevice &cataclysmDevice) : cataclysmDevice{cataclysmDevice} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<CataclysmDescriptorPool> build() const;

        private:
            CataclysmDevice &cataclysmDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        CataclysmDescriptorPool(
            CataclysmDevice &cataclysmDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~CataclysmDescriptorPool();
        CataclysmDescriptorPool(const CataclysmDescriptorPool &) = delete;
        CataclysmDescriptorPool &operator=(const CataclysmDescriptorPool &) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

    private:
        CataclysmDevice &cataclysmDevice;
        VkDescriptorPool descriptorPool;

        friend class CataclysmDescriptorWriter;
    };

    class CataclysmDescriptorWriter
    {
    public:
        CataclysmDescriptorWriter(CataclysmDescriptorSetLayout &setLayout, CataclysmDescriptorPool &pool);

        CataclysmDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        CataclysmDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        CataclysmDescriptorSetLayout &setLayout;
        CataclysmDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

} // namespace Cataclysm