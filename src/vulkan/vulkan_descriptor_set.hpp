#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/descriptor_set.hpp"
#include "resource/vulkan_buffer.hpp"
#include "resource/vulkan_texture.hpp"

namespace MangoRHI {
    class VulkanDescriptorSet;

    class VulkanDescriptor : public RuntimeComponent {
    public:
        virtual void update(VulkanDescriptorSet *descriptor_set) = 0;
    
    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, VkDescriptorSetLayoutBinding, binding, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, binding_index, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, count, MANGO_NO_INIT_VAULE)
    };

    class VulkanUniformDescriptor final : public VulkanDescriptor {
    public:
        Result create() override;
        Result destroy() override;

        void update(VulkanDescriptorSet *descriptor_set) override;
    
    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, size, MANGO_NO_INIT_VAULE)

    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, VulkanBuffer, buffer, MANGO_NO_INIT_VAULE)
    
    declare_component_cls(VulkanUniformDescriptor)
    };

    class VulkanTextureDescriptor final : public VulkanDescriptor {
    public:
        void add_texture(VulkanTexture *texture);
        void set_texture(u32 index, VulkanTexture *texture);

        Result create() override;
        Result destroy() override;

        void update(VulkanDescriptorSet *descriptor_set) override;
    
    define_private_member(STL_IMPL::vector<VulkanTexture *>, textures, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanTextureDescriptor)
    };

    extern STL_IMPL::unordered_map<VkDescriptorType, u32> g_descriptor_info;

    class VulkanDescriptorPool;
    
    class VulkanDescriptorSet final : public DescriptorSet {
        friend VulkanDescriptorPool;
    public:
        VulkanDescriptorSet();
        void add_uniform(DescriptorStage stage, u32 size, u32 count) override;
        void add_textures(DescriptorStage stage, Texture **textures, u32 count) override;
        
        Result create() override;
        Result destroy() override;
        
        void *map_uniform_buffer_pointer(u32 binding, u32 index) override;
        void set_texture(u32 binding, u32 index, Texture *texture) override;
        void update() override;

    private:
        void setup_descriptor_binding(VulkanDescriptor *descriptor, VkDescriptorType type, DescriptorStage stage, u32 count);
    
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkDescriptorSetLayout, layout, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkDescriptorSet, descriptor_set, VK_NULL_HANDLE)
    define_private_member(STL_IMPL::vector<VulkanDescriptor *>, descriptors, MANGO_NO_INIT_VAULE)
    define_private_member(u32, _current_binding, 0)
    };

    extern STL_IMPL::vector<VulkanDescriptorSet *> g_vulkan_descriptor_sets;
}
