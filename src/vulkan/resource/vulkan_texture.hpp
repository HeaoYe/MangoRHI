#pragma once

#include "../vulkan_commons.hpp"
#include "MangoRHI/resource/texture.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_sampler.hpp"
#include "vulkan_image.hpp"

namespace MangoRHI {
    class VulkanTexture final : public Texture {
    public:
        void set_filename(const char *filename) override;
        void bind_sampler(Sampler *sampler) override;

        Result create() override;
        Result destroy() override;

    define_private_readonly_pointer(char, filename, "")
    define_private_member(i32, width, MANGO_NO_INIT_VAULE)
    define_private_member(i32, height, MANGO_NO_INIT_VAULE)
    define_private_member(i32, channels, MANGO_NO_INIT_VAULE)
    define_private_member(VulkanBuffer, staging, MANGO_NO_INIT_VAULE)
    define_member(VulkanImage, image, MANGO_NO_INIT_VAULE)
    define_pointer(VulkanSampler, sampler, MANGO_NO_INIT_VAULE)

    no_copy_and_move_construction(VulkanTexture)
    };
}
