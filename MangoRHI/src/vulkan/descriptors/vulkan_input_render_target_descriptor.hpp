#pragma once

#include "vulkan_descriptor.hpp"
#include "../resource/vulkan_render_target.hpp"
#include "../resource/vulkan_sampler.hpp"

namespace MangoRHI {
    class VulkanInputRenderTargetDescriptor final : public VulkanDescriptor {
    public:
        void update(const VulkanDescriptorSet &descriptor_set) override;
        DescriptorType get_type() const override { return DescriptorType::eInputRenderTarget; };

        void add_render_target(const VulkanRenderTarget &render_target, const VulkanSampler &sampler);
        void set_render_target(u32 index, const VulkanRenderTarget &render_target, const VulkanSampler &sampler);

    define_private_member(STL_IMPL::vector<Ref<const VulkanRenderTarget>>, render_targets, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<Ref<const VulkanSampler>>, samplers, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanInputRenderTargetDescriptor)
    };

}
