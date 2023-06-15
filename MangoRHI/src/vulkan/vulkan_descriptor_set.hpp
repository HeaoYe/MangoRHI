#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/descriptor_set.hpp"
#include "descriptors/vulkan_descriptor.hpp"

namespace MangoRHI {

    class VulkanDescriptorPool;

    class VulkanDescriptorSetLayout final : public DescriptorSetLayout {
    public:
        u32 add_uniforms_descriptor(DescriptorStageFlags stages, u32 size, u32 count) override;
        u32 add_textures_descriptor(DescriptorStageFlags stages, u32 count) override;
        u32 add_input_render_targets_descriptor(DescriptorStageFlags stages, u32 count) override;

    define_readonly_pointer(MANGO_CONST_GETTER, MANGO_SETTER_OVERRIDE, char, name, "")
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VulkanDescriptorBinding>, bindings, MANGO_NO_INIT_VAULE)

    define_private_member(u32, _current_binding, 0)

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkDescriptorSetLayout, layout, VK_NULL_HANDLE)

    declare_component_cls(VulkanDescriptorSetLayout)
    };

    class VulkanDescriptorSet final : public DescriptorSet {
        friend VulkanDescriptorPool;
    public:
        void *get_uniform_buffer_pointer(u32 binding, u32 index) override;
        void set_texture(u32 binding, u32 index, Texture &texture) override;
        void set_input_render_target(u32 binding, u32 index, const STL_IMPL::pair<const char *, const Sampler &> &render_target) override;
        void update(u32 binding) override;
        void update_all() override;

        const VkDescriptorSet &get_current_in_flight_descriptor_set() const;

    define_member(MANGO_NO_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<std::unique_ptr<VulkanDescriptor>>, descriptors, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkDescriptorSet>, in_flight_descriptor_sets, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanDescriptorSet)
    };
}
