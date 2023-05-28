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
    
    define_extern_writeable_member(VkDescriptorSetLayoutBinding, binding, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(u32, n_binding, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(u32, count, MANGO_NO_INIT_VAULE)
    };

    class VulkanUniformDescriptor final : public VulkanDescriptor {
    public:
        Result create() override;
        Result destroy() override;

        void update(VulkanDescriptorSet *descriptor_set) override;

    define_extern_writeable_member(VulkanBuffer, buffer, MANGO_NO_INIT_VAULE)
    define_extern_writeable_member(u32, size, MANGO_NO_INIT_VAULE)
    
    no_copy_and_move_construction(VulkanUniformDescriptor)
    };

    class VulkanTextureDescriptor final : public VulkanDescriptor {
    public:
        void add_texture(VulkanTexture *texture);

        Result create() override;
        Result destroy() override;

        void update(VulkanDescriptorSet *descriptor_set) override;
    
    define_member(STL_IMPL::vector<VulkanTexture *>, textures, MANGO_NO_INIT_VAULE)

    no_copy_and_move_construction(VulkanTextureDescriptor)
    };

    extern STL_IMPL::unordered_map<VkDescriptorType, u32> g_descriptor_info;

    class VulkanDescriptorSet final : public DescriptorSet {
    public:
        VulkanDescriptorSet();
        void add_uniform(DescriptorStage stage, u32 size, u32 count) override;
        void add_textures(DescriptorStage stage, Texture **textures, u32 count) override;
        
        Result create() override;
        Result destroy() override;
        
        void *map_uniform_buffer_pointer(u32 binding, u32 index) override;
        void update();

    private:
        VkDescriptorSetLayoutBinding &setup_descriptor_binding(VulkanDescriptor *descriptor, VkDescriptorType type, DescriptorStage stage, u32 count);
    
    define_member(STL_IMPL::vector<VulkanDescriptor *>, descriptors, MANGO_NO_INIT_VAULE)
    define_member(VkDescriptorSetLayout, layout, VK_NULL_HANDLE)
    define_extern_writeable_member(VkDescriptorSet, descriptor_set, VK_NULL_HANDLE)
    define_private_member(u32, _current_binding, 0)
    };

    extern STL_IMPL::vector<VulkanDescriptorSet *> g_vulkan_descriptor_sets;
}
