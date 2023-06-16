#pragma once

#include "vulkan_descriptor.hpp"
#include "../resource/vulkan_texture.hpp"

namespace MangoRHI {
    class VulkanTextureDescriptor final : public VulkanDescriptor {
    public:
        void update(const VulkanDescriptorSet &descriptor_set) override;
        DescriptorType get_type() const override { return DescriptorType::eTexture; };

        void add_texture(const VulkanTexture &texture);
        void set_texture(u32 index, const VulkanTexture &texture);

    define_private_member(STL_IMPL::vector<Ref<const VulkanTexture>>, textures, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanTextureDescriptor)
    };

}
