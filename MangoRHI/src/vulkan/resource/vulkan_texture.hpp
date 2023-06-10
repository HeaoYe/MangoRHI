#pragma once

#include "../vulkan_commons.hpp"
#include "MangoRHI/resource/texture.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_sampler.hpp"
#include "vulkan_image.hpp"

namespace MangoRHI {
    class VulkanTexture final : public Texture {
    public:
        Sampler &get_sampler_reference() override;

    define_readonly_pointer(MANGO_NO_GETTER, MANGO_SETTER_OVERRIDE, char, filename, "")
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC_OVERRIDE, Bool, is_empty, MG_FALSE)
    define_member(MANGO_NO_GETTER, MANGO_SETTER_BASIC_OVERRIDE, u32, mipmap_levels, 0)

    define_private_member(VkExtent2D, extent, MANGO_NO_INIT_VAULE)
    define_private_member(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanImage, image, MANGO_NO_INIT_VAULE)
    define_member(MANGO_CONST_GETTER, MANGO_NO_SETTER, VulkanSampler, sampler, MANGO_NO_INIT_VAULE)

    declare_component_cls(VulkanTexture)
    };
}
