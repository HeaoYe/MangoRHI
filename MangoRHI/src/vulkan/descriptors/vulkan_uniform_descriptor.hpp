#pragma once

#include "vulkan_descriptor.hpp"
#include "../resource/vulkan_buffer.hpp"

namespace MangoRHI {
    class VulkanUniformDescriptor final : public VulkanDescriptor {
    public:
        void update(const VulkanDescriptorSet &descriptor_set) override;
        DescriptorType get_type() const override { return DescriptorType::eUniform; };

        void *get_current_mapped_pointer() const;

    define_member(MANGO_CONST_GETTER, MANGO_SETTER_BASIC, u32, size, MANGO_NO_INIT_VAULE)

    define_private_member(STL_IMPL::vector<std::unique_ptr<VulkanBuffer>>, in_flight_buffers, MANGO_NO_INIT_VAULE)
    define_private_member(STL_IMPL::vector<void *>, mapped_pointers, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanUniformDescriptor)
    };
}
