#pragma once

#include "vulkan_commons.hpp"
#include "MangoRHI/descriptor_set.hpp"
#include "resource/vulkan_buffer.hpp"
#include "resource/vulkan_texture.hpp"
#include "resource/vulkan_render_target.hpp"

namespace MangoRHI {
    class VulkanDescriptorSet;

    enum class DescriptorType {
        eNone,
        eUniform,
        eTexture,
        eInputRenderTarget,
    };

    class VulkanDescriptor : public RuntimeComponent {
    public:
        virtual void update(const VulkanDescriptorSet &descriptor_set) = 0;
        virtual DescriptorType get_type() const = 0;

    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, VkDescriptorSetLayoutBinding, binding, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, binding_index, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, count, MANGO_NO_INIT_VAULE)
    };

    class VulkanUniformDescriptor final : public VulkanDescriptor {
    public:
        void update(const VulkanDescriptorSet &descriptor_set) override;
        DescriptorType get_type() const override { return DescriptorType::eUniform; };

        void *get_current_mapped_pointer() const;

    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, size, MANGO_NO_INIT_VAULE)

    define_private_member(STL_IMPL::vector<Reference<VulkanBuffer>>, in_flight_buffers, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<void *>, mapped_pointers, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanUniformDescriptor)
    };

    class VulkanTextureDescriptor final : public VulkanDescriptor {
    public:
        void update(const VulkanDescriptorSet &descriptor_set) override;
        DescriptorType get_type() const override { return DescriptorType::eTexture; };

        void add_texture(const VulkanTexture &texture);
        void set_texture(u32 index, const VulkanTexture &texture);

    define_private_member(STL_IMPL::vector<Reference<const VulkanTexture>>, textures, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanTextureDescriptor)
    };

    class VulkanInputRenderTargetDescriptor final : public VulkanDescriptor {
    public:
        void update(const VulkanDescriptorSet &descriptor_set) override;
        DescriptorType get_type() const override { return DescriptorType::eInputRenderTarget; };

        void add_render_target(const VulkanRenderTarget &render_target, const VulkanSampler &sampler);
        void set_render_target(u32 index, const VulkanRenderTarget &render_target, const VulkanSampler &sampler);

    define_private_member(STL_IMPL::vector<Reference<const VulkanRenderTarget>>, render_targets, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<Reference<const VulkanSampler>>, samplers, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanInputRenderTargetDescriptor)
    };

    extern STL_IMPL::unordered_map<VkDescriptorType, u32> g_descriptor_info;

    class VulkanDescriptorPool;

    class VulkanDescriptorSet final : public DescriptorSet {
        friend VulkanDescriptorPool;
    public:
        u32 add_uniforms(DescriptorStage stage, u32 size, u32 count) override;
        u32 add_textures(DescriptorStage stage, const STL_IMPL::vector<Reference<Texture>> &textures) override;
        u32 add_input_render_targets(DescriptorStage stage, const STL_IMPL::vector<STL_IMPL::pair<const char *, Sampler &>> &render_targets) override;

        void *get_uniform_buffer_pointer(u32 binding, u32 index) override;
        void set_texture(u32 binding, u32 index, Texture &texture) override;
        void set_input_render_target(u32 binding, u32 index, const STL_IMPL::pair<const char *, Sampler &> &render_target) override;
        void update() override;

    private:
        void setup_descriptor_binding(VulkanDescriptor &descriptor, VkDescriptorType type, DescriptorStage stage, u32 count);

    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VkDescriptorSetLayout, layout, VK_NULL_HANDLE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<VkDescriptorSet>, in_flight_descriptor_sets, MANGO_NO_INIT_VAULE)
    define_member(MANGO_MUTABLE_GETTER, MANGO_NO_SETTER, STL_IMPL::vector<Reference<VulkanDescriptor>>, descriptors, MANGO_NO_INIT_VAULE)
    define_private_member(u32, _current_binding, 0)

    declare_component_cls_custom_construction(VulkanDescriptorSet)
    };

    extern STL_IMPL::vector<Reference<VulkanDescriptorSet>> g_vulkan_descriptor_sets;
}
